#include "core/glTFLoader.h"
#include "core/texture.h"
#include "core/material.h"
#include "core/primitive.h"
#include "shape/triangle.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>

using namespace rapidjson;

void layoutFromAccessor(LayoutItem& l, const Value& accessor, const Value& bufferViews, std::vector<char*>& buffers) {
      l.offset = 0;
      const Value& bufferView = bufferViews[accessor["bufferView"].GetUint()];
      uint32_t bufferVOffset = 0, accessorOffset = 0;
      auto it = bufferView.FindMember("byteOffset");
      if (it != bufferView.MemberEnd())
            bufferVOffset = it->value.GetUint();
      it = accessor.FindMember("byteOffset");
      if (it != accessor.MemberEnd())
            accessorOffset = it->value.GetUint();
      uint32_t offset = bufferVOffset + accessorOffset;
      l.data_ptr = buffers[bufferView["buffer"].GetUint()] + offset;
      it = accessor.FindMember("normalized");
      if (it != accessor.MemberEnd())
            l.normalized = it->value.GetBool() ? GL_TRUE : GL_FALSE;
      uint32_t eType = accessor["componentType"].GetUint();
      // TODO: direct copy element type!
      switch (eType) {
      case ElementType::BYTE:
            l.e_size = 1;
            l.e_format = GL_BYTE; break;
      case ElementType::FLOAT:
            l.e_size = 4;
            l.e_format = GL_FLOAT; break;
      case ElementType::SHORT:
            l.e_size = 2;
            l.e_format = GL_SHORT; break;
      case ElementType::UNSIGNED_BYTE:
            l.e_size = 1;
            l.e_format = GL_UNSIGNED_BYTE; break;
      case ElementType::UNSIGNED_INT:
            l.e_size = 4;
            l.e_format = GL_UNSIGNED_INT; break;
      case ElementType::UNSIGNED_SHORT:
            l.e_size = 2;
            l.e_format = GL_UNSIGNED_SHORT; break;
      default:
            break;
      }
            

      std::string type_str = accessor["type"].GetString();
      if (type_str == "SCALAR")
            l.e_count = 1;
      else if (type_str == "VEC2")
            l.e_count = 2;
      else if (type_str == "VEC3")
            l.e_count = 3;
      else if (type_str == "VEC4")
            l.e_count = 4;

      uint32_t stride;
      it = bufferView.FindMember("byteStride");
      if (it != bufferView.MemberEnd())
            stride = it->value.GetUint();
      else
            stride = l.e_size*l.e_count;
      l.strip = stride;
}

char* collectVertexAttributes(const Value& attributes, const Value& accessors, const Value& bufferViews, std::vector<char*>& buffers,
      Layout& vbLayout, uint32_t& vertexNum) {
      std::vector<LayoutItem> layouts;
      vertexNum = 0;
      for (auto it = attributes.MemberBegin(); it != attributes.MemberEnd(); it++) {
            LayoutItem l;
            std::string attrib = it->name.GetString();
            if (attrib == "POSITION")
                  l.type = ARRAY_VERTEX;
            else if (attrib == "TEXCOORD_0")
                  l.type = ARRAY_TEX_UV;
            else if (attrib == "TEXCOORD_1")
                  l.type = ARRAY_TEX_UV2;
            else if (attrib == "NORMAL")
                  l.type = ARRAY_NORMAL;
            else if (attrib == "TANGENT")
                  l.type = ARRAY_TANGENT;
            const Value& accessor = accessors[it->value.GetUint()];
            if (vertexNum == 0)
                  vertexNum = accessor["count"].GetUint();
            layoutFromAccessor(l, accessor, bufferViews, buffers);
            layouts.push_back(l);
      }
      vbLayout = Layout(layouts);
      uint32_t compatStrip = vbLayout.strip();
      // copy from bin buffer
      char* vertexBuffer = new char[vbLayout.strip()*vertexNum];
      for (int i = 0; i < layouts.size(); i++) {
            auto& l = layouts[i];
            uint32_t offset = vbLayout[i].offset;
            for (int j = 0; j < vertexNum; j++) {
                  // here l.strip is the one from the original buffer, NOT the vertex buffer
                  char* src = (char*)l.data_ptr + j * l.strip;
                  char* dst = vertexBuffer + j * compatStrip + offset;
                  std::memcpy(dst, src, l.e_count*l.e_size);
            }
      }
      return vertexBuffer;
}

std::vector<Primitive*> LoadGLTF(std::string path) {
      std::string dir;
      int dirStart = path.find_last_of('/');
      if (dirStart != std::string::npos && dirStart < path.size() - 1)
            //dir = path.substr(dirStart + 1);
      dir = path.substr(0, dirStart);
      std::string gltfFileStr;
      std::stringstream gltfFileStream;
      std::ifstream gltfFile(path);
      gltfFileStream << gltfFile.rdbuf();
      gltfFileStr = gltfFileStream.str();
      Document d;
      d.Parse(gltfFileStr.c_str());

      // load bin files
      const Value& buffers_json = d["buffers"];
      std::vector<char*> buffer_array(buffers_json.Size());
      for (int i = 0; i < buffers_json.Size(); i++) {
            std::ifstream fbuffer(dir + '/' + buffers_json[i]["uri"].GetString(), std::ios_base::binary);
            bool ok = fbuffer.is_open();
            uint32_t byteLength = buffers_json[i]["byteLength"].GetUint();
            char* bin_data = new char[byteLength];
            fbuffer.read(bin_data, byteLength);
            buffer_array[i] = bin_data;
      }
      

      // create textures
      std::vector<ImageTexture> textures;
      {
            const Value& texture_json = d["textures"];
            assert(texture_json.IsArray());
            for (int i = 0; i < texture_json.Size(); i++) {
                  int img_idx = texture_json[i]["source"].GetUint();
                  std::string img_path(d["images"][img_idx]["uri"].GetString());
                  img_path = dir + '/' + img_path;
                  Image* img = new Image(img_path, Image::RGBSpectrum, false);
                  textures.emplace_back(img);
                  // TODO: read texture.sampler
            }
      }
      
      std::vector<PBRMaterial> materials;
      {
            const Value& materials_json = d["materials"];
            assert(materials_json.IsArray());
            for (int i = 0; i < materials_json.Size(); i++) {
                  PBRMaterial m;
                  const Value& material_js = materials_json[i];
                  if (material_js.FindMember("pbrMetallicRoughness") == material_js.MemberEnd())
                        continue;
                  const Value& pbr_js = material_js["pbrMetallicRoughness"];
                  // factors
                  Value::ConstMemberIterator it = pbr_js.FindMember("baseColorFactor");
                  if (it != pbr_js.MemberEnd()) {
                        for (int j = 0; j < 3; j++)
                              m.albedoFactor.rgb[j] = it->value[j].GetFloat();
                  }
                  it = pbr_js.FindMember("metallicFactor");
                  if (it != pbr_js.MemberEnd()) {
                        m.metallicFactor = it->value.GetFloat();
                  }
                  it = pbr_js.FindMember("roughnessFactor");
                  if (it != pbr_js.MemberEnd()) {
                        m.roughFactor = it->value.GetFloat();
                  }
                  // textures
                  it = pbr_js.FindMember("baseColorTexture");
                  if (it != pbr_js.MemberEnd()) {
                        m.albedo_map = textures[it->value["index"].GetUint()];
                  }
                  it = pbr_js.FindMember("metallicRoughnessTexture");
                  if (it != pbr_js.MemberEnd()) {
                        m.metallicRoughnessMap = textures[it->value["index"].GetUint()];
                  }
                  it = material_js.FindMember("emissiveTexture");
                  if (it != material_js.MemberEnd()) {
                        m.emissive_map = textures[it->value["index"].GetUint()];
                  }
                  it = material_js.FindMember("emissiveFactor");
                  if (it != material_js.MemberEnd()) {
                        for (int j = 0; j < 3; j++)
                              m.emissiveFactor.rgb[j] = it->value[j].GetFloat();
                  }
                  it = material_js.FindMember("normalTexture");
                  if (it != material_js.MemberEnd()) {
                        m.normal_map = textures[it->value["index"].GetUint()];
                  }
                  it = material_js.FindMember("alphaMode");
                  if (it != material_js.MemberEnd()) {
                        if (std::string("OPACITY") == it->value.GetString())
                              m.alphaMode = OPACITY;
                        else if (std::string("MASK") == it->value.GetString())
                              m.alphaMode = MASK;
                        else if (std::string("BLEND") == it->value.GetString())
                              m.alphaMode = BLEND;
                  }
                  materials.push_back(m);
            }
      }
      // nodes
      // only the first scene is parsed
      const Value& nodes_array = d["scenes"][0]["nodes"];
      const Value& nodes = d["nodes"];
      const Value& glTFmeshes = d["meshes"];
      const Value& accessors = d["accessors"];
      const Value& bufferViews = d["bufferViews"];
      std::vector<Primitive*> primitives;
      for (int i = 0; i < nodes_array.Size(); i++) {
            // each node contains multiple meshes, so here treated as a primitive
            uint32_t node_idx = nodes_array[i].GetUint();
            uint32_t mesh_idx = nodes[node_idx]["mesh"].GetUint();
            const Value& p_meshes = glTFmeshes[mesh_idx]["primitives"];
            std::vector<TriangleMesh*> meshes;
            std::vector<PBRMaterial> rtms;
            for (int j = 0; j < p_meshes.Size(); j++) {
                  const Value& mesh = p_meshes[j];
                  rtms.push_back(materials[mesh["material"].GetUint()]);
                  auto it = mesh.FindMember("mode");
                  GLenum primitive_mode = GL_TRIANGLES;
                  if(it != mesh.MemberEnd())
                        primitive_mode = mesh["mode"].GetUint();
                  // index
                  const Value& index_accessor = accessors[mesh["indices"].GetUint()];
                  uint32_t indexNum = index_accessor["count"].GetUint();
                  const Value& index_bufferView = bufferViews[index_accessor["bufferView"].GetUint()];
                  LayoutItem indexLayout;
                  layoutFromAccessor(indexLayout, index_accessor, bufferViews, buffer_array);
                  char* index_data = new char[indexLayout.e_size*indexLayout.e_count*indexNum];
                  // here we assume the index data is compact
                  std::memcpy(index_data, indexLayout.data_ptr, indexLayout.e_size*indexLayout.e_count*indexNum);
                  GLenum indexElementT = index_accessor["componentType"].GetUint();
                  // construct vertex buffer layout
                  Layout vbLayout;
                  uint32_t vertexNum;
                  char* vertex_data = collectVertexAttributes(mesh["attributes"], accessors, bufferViews, buffer_array, vbLayout, vertexNum);
                  meshes.push_back(new TriangleMesh(vertex_data, vbLayout, vertexNum, index_data, indexNum / 3, indexElementT, Transform::Identity()));
            }
            Primitive* p = new Primitive(rtms, meshes);
            primitives.push_back(p);
      }
      for (char* p : buffer_array)
            delete p;
      return primitives;
}