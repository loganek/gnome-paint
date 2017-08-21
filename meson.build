project('gnome-paint', 'c',
  license: 'GPL3',
  version: '0.1.0',
  meson_version: '>= 0.40.1',
  default_options: [
    'c_std=gnu11',
    'cpp_std=c++11',
    'warning_level=3',
  ],
)

conf = configuration_data()
conf.set_quoted('PACKAGE_NAME', 'gnome-paint')
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('PACKAGE_STRING', 'gnome-paint-' + meson.project_version())
conf.set_quoted('PACKAGE_DATADIR', join_paths(get_option('prefix'), get_option('datadir')))
conf.set_quoted('PACKAGE_DOCDIR', join_paths(get_option('prefix'), get_option('datadir'), 'doc/gnome-paint'))
conf.set_quoted('PACKAGE_LIBDIR', join_paths(get_option('prefix'), get_option('libdir')))
conf.set_quoted('PACKAGE_LOCALE_DIR', join_paths(get_option('prefix'), get_option('datadir'), 'locale'))
conf.set_quoted('PACKAGE_LIBEXECDIR', join_paths(get_option('prefix'), get_option('libexecdir')))
conf.set_quoted('PACKAGE_URL', 'https://bugzilla.gnome.org/enter_bug.cgi?product=gnome-paint')
conf.set('GETTEXT_PACKAGE', 'PACKAGE_NAME')
conf.set('LOCALEDIR', 'PACKAGE_LOCALE_DIR')

configure_file(output : 'config.h',
               configuration : conf)

config_inc_dir = include_directories('.')

global_c_args = [
  '-Wno-unused-parameter'
]

add_project_arguments(global_c_args, language: 'c')

subdir('src')
subdir('po')