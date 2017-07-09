gnome = import('gnome')

gtkdep = dependency('gtk+-3.0')

gp_resources = gnome.compile_resources(
  'gp-resources', join_paths(meson.source_root(), 'data', 'org.gnome.Paint.gresource.xml'),
  source_dir : join_paths(meson.source_root(), 'data'),
  c_name : 'gp')

gp_sources = [
  'gp-application.c',
  'gp-cairoutils.c',
  'gp-colorbutton.c',
  'gp-colormanager.c',
  'gp-colorselectorbox.c',
  'gp-dialogutils.c',
  'gp-document.c',
  'gp-documentmanager.c',
  'gp-drawhistoryitem.c',
  'gp-drawingarea.c',
  'gp-headerbar.c',
  'gp-help-commands.c',
  'gp-history.c',
  'gp-historyitem.c',
  'gp-imageeditor.c',
  'gp-linetool.c',
  'gp-main.c',
  'gp-marshal.c',
  'gp-propertybox.c',
  'gp-rectangleselectiontool.c',
  'gp-rectangletool.c',
  gp_resources,
  'gp-shapetool.c',
  'gp-sizetoolproperty.c',
  'gp-toolbox.c',
  'gp-tool.c',
  'gp-toolmanager.c',
  'gp-toolproperty.c',
  'gp-widget-init.c',
  'gp-window.c',
  'gp-window-commands.c',
  ]

executable('gnome-paint', gp_sources, dependencies: gtkdep, include_directories: config_inc_dir)
