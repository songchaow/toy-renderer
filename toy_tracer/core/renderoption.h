#pragma once
struct RenderOption {
      // how many pixels one block contains
      //int pixel_per_block = 1;
      int sample_per_pixel;
      // using which sampler?
      //    At most, 2 samplers are required for now.
      // using which filter?
      // for path tracing:
      int max_bounce_count;
};