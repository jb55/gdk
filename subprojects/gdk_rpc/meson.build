project('gdk_rpc', ['c'], meson_version : '>= 0.51.2')

message('Building gdk_rpc...')

run_cmd_args = []

target_os = meson.get_cross_property('target_os', '')
if target_os != ''
    run_cmd_args += ['--' + target_os]
endif

gdk_rpc_deps = []
gdk_rpc_deps += subproject('libwally-core')

res = run_command(join_paths(meson.source_root(), 'tools', 'buildgdk_rpc.sh'), run_cmd_args)
if res.returncode() != 0
    message('--- Failed to run command (stdout) ---')
    message(res.stdout())
    message('--- Failed to run command (stderr) ---')
    message(res.stderr())
endif

gdk_rpc_dir_name = 'gdk_rpc'
incdir = join_paths('..', '..', meson.build_root().split('/')[-1], gdk_rpc_dir_name)

libdir = join_paths(meson.build_root(), gdk_rpc_dir_name, 'target', 'release')
all_libs = []

compiler = meson.get_compiler('c')
gdk_rpc_lib = compiler.find_library('gdk_rpc', dirs : libdir)
all_libs += gdk_rpc_lib

gdk_rpc_dep = declare_dependency(
    include_directories : include_directories(incdir),
    dependencies : all_libs)
