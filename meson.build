
project('wave_func_collapse', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

incdir = include_directories(['include', '.'])


util_src_files = []
src_files = ['source/main.cpp', 'utils/dynamic_bitset.cpp', 'source/wave.cpp', 'source/wave_input_data.cpp']

exe = executable('wfc', src_files,
  install : true, include_directories : incdir)

test('basic', exe)
