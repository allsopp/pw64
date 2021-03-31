project('pw64', 'c',
  version: '0.2.0',
  default_options: [ 'c_std=c99', 'warning_level=3', 'werror=true' ])

src = ['base64.c', 'password.c', 'main.c']

deps = [ dependency('nettle', version: '>=3.4') ]

config = configuration_data()
config.set('version', meson.project_version())
config.set('name', meson.project_name())
configure_file(
  input: 'config.h.in',
  output: 'config.h',
  configuration: config
)

pw64 = executable('pw64', src, dependencies: deps, install: true)
subdir('tests')
