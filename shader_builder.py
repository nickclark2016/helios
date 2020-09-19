#!/usr/bin/python

import argparse
import os


# As found in: https://stackoverflow.com/a/59803793/3854096
def run_fast_scandir(dir, ext):
    subfolders, files = [], []

    for f in os.scandir(dir):
        if f.is_dir():
            subfolders.append(f.path)
        if f.is_file():
            if os.path.splitext(f.name)[1].lower() in ext:
                files.append(f.path)

    for dir in list(subfolders):
        sf, f = run_fast_scandir(dir, ext)
        subfolders.extend(sf)
        files.extend(f)

    return subfolders, files


def compile_single(shader_path, type, output_name, reflect, verbose):
    shader_dir = os.path.dirname(shader_path)
    output = shader_dir + os.path.sep + output_name
    if verbose is True:
        print('Compiling {} to {}'.format(shader_path, output))
    cmd = 'glslc {} -fshader-stage={} -o {}'.format(shader_path, type, output)
    os.system(cmd)

    if reflect is True:
        cmd = 'spirv-cross {} --reflect --output {}'.format(
            output, output + '.json')
        if verbose is True:
            print('Reflecting {} to {}'.format(output, output + '.json'))
        os.system(cmd)


def compile(folder, reflect, verbose):
    vertex_shaders = run_fast_scandir(folder, ['.vert'])[1]
    for shader in vertex_shaders:
        compile_single(shader, 'vertex', 'vert.spv', reflect, verbose)

    fragment_shaders = run_fast_scandir(folder, ['.frag'])[1]
    for shader in fragment_shaders:
        compile_single(shader, 'fragment', 'frag.spv', reflect, verbose)


def main():
    parser = argparse.ArgumentParser(
        description='Shader Compilation and Reflection Generation')
    parser.add_argument('directory', metavar='Directory',
                        type=str, help='Directory to compile shaders from.')
    parser.add_argument('--reflect', action='store_true',
                        default=False, help='Generate reflection files.')
    parser.add_argument('--verbose', action='store_true',
                        default=False, help='Verbose output.')

    args = parser.parse_args()
    compile(args.directory, args.reflect, args.verbose)


if __name__ == "__main__":
    main()
