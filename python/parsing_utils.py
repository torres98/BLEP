import argparse

def parse_args(parse_sample_size = False):
    def positive_int(v):
        value = int(v)

        if value <= 0:
            raise argparse.ArgumentTypeError(f'{v} is not positive')

        return value

    boards_info = {
        'cc1352r1_launchxl': ['west', 'flash', '--skip-rebuild'],
        'nrf5340dk_nrf5340_cpuapp': ['west', 'flash', '--recover', '--skip-rebuild'],
        'arduino_due': ['west', 'flash', '--skip-rebuild'],
        'adafruit_itsybitsy_m4_express': ['west', 'flash', '--skip-rebuild'],
        'rpi_pico': ['cp', 'build/zephyr/zephyr.uf2', '/media/torres/RPI-RP2'],
        'esp32': ['west', 'flash', '--skip-rebuild'],
        'qemu_x86': ['west', 'build', '-t', 'run'],
        'qemu_riscv32': ['west', 'build', '-t', 'run'],
        'qemu_cortex_r5': ['west', 'build', '-t', 'run']
    }

    n_polynomials = {
        1: 64,
        2: 80,
        3: 100
    }   

    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--rainbow-version', choices=(1, 2, 3), default=1, type=int)
    parser.add_argument('-k', '--svk-nrows', default=1, type=positive_int)
    parser.add_argument('-t', '--progressive-steps', default=1, type=positive_int)

    if parse_sample_size:
        parser.add_argument('-s', '--sample-size', default=100, type=positive_int)

    parser.add_argument('-b', '--board', choices=boards_info.keys(), type=str)
    parser.add_argument('-l', '--lookup-level', choices=('0', '1', '2', '1r', '2r'), default='0', type=str)
    parser.add_argument('--skip-build', action='store_true')

    args = parser.parse_args()

    if args.svk_nrows > n_polynomials[args.rainbow_version]:
        parser.error(f'argument k: invalid choice: {args.svk_nrows} (can\'t be bigger than {n_polynomials[args.rainbow_version]})')

    if args.progressive_steps > args.svk_nrows:
        parser.error(f'argument t: invalid choice: {args.progressive_steps} (can\'t be bigger than argument k)')

    args_dict = {
        'RAINBOW_VERSION': args.rainbow_version,
        'SVK_NROWS': args.svk_nrows,
        'PROGRESSIVE_STEPS': args.progressive_steps,
        'LOOKUP_LEVEL': args.lookup_level,
        'BOARD': args.board,
        'FLASH_CMD': boards_info[args.board],
        'SKIP_BUILD': args.skip_build
    }

    if parse_sample_size:
        args_dict['SAMPLE_SIZE'] = args.sample_size

    return args_dict
