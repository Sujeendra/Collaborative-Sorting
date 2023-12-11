file(REMOVE_RECURSE
  "../src/ik_trajectory_tutorial/srv"
  "../srv_gen"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/rosbuild_precompile.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
