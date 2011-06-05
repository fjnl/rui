# -*- python -*-

def options(opt):
    opt.load('compiler_cxx unittestt')
    opt.add_option(
        '--enable-optimized',
        action = 'store',
        default = False
    )
    opt.add_option(
        '--disable-assert',
        action = 'store',
        default = False
    )
    opt.recurse('rui')

def configure(conf):
    def append_flags(*flags):
        conf.env.append_unique('CFLAGS', flags)
        conf.env.append_unique('CXXFLAGS', flags)

    conf.load('compiler_cxx unittestt')

    append_flags('-g', '-Wextra', '-Wall')
    if conf.options.enable_optimized:
        append_flags('-O2')
    if conf.options.disable_assert:
        append_flags('-DNDEBUG')

    conf.recurse('rui')

def build(bld):
    include = bld.path.find_dir('include')
    bld.install_files('${PREFIX}', include.ant_glob('**/*'), relative_trick = True)
    bld.recurse('rui')
