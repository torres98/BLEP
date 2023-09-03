import argparse
import subprocess

GF_LOOKUP_LEVELS_MACRO_VALUES = {
    '0': 0,
    '1': 1,
    '1r': 2,
    '2': 3,
    '2r': 4
}

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--rainbow-version', choices=(1, 2, 3), default=1, type=int)
    parser.add_argument('-l', '--lookup-level', choices=('0', '1', '2', '1r', '2r'), default='0', type=str)
    parser.add_argument('-t', '--enable-tests', action="store_true")

    parsed_args = parser.parse_args()

    subprocess.run(
        ['cmake', '.', '-B', 'build/',
         f'-DRAINBOW_VERSION={parsed_args.rainbow_version}',
         f'-DGF16_LOOKUP={GF_LOOKUP_LEVELS_MACRO_VALUES[parsed_args.lookup_level]}',
         f'-DGF256_LOOKUP={parsed_args.lookup_level}',
         '-DENABLE_TESTS=1' if parsed_args.enable_tests else '']
    )
    
    subprocess.run(['cmake', '--build', 'build/', '--target', 'all'])