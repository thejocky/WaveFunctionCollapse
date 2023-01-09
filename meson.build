
project('wave_func_collapse', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++14'])

incdir = include_directories('include')

src_files = ['source/wave.cpp']

exe = executable('wfc', src_files,
  install : true, include_directories : incdir)

test('basic', exe)
