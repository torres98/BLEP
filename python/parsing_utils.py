import argparse

boards_info = {
    'cc1352r1_launchxl': ('/dev/ttyACM0', ['west', 'flash']),
    'nrf5340dk_nrf5340_cpuapp': ('/dev/ttyACM2', ['west', 'flash', '--recover'])
}

n_polynomials = {
    1: 64,
    2: 80,
    3: 100
}

def parse_args():
    def positive_int(v):
        value = int(v)

        if value <= 0:
            raise argparse.ArgumentTypeError(f'{v} is not positive')

        return value

    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--version', choices=(1, 2, 3), default=1, type=int)
    parser.add_argument('-b', '--board', choices=boards_info.keys(), type=str)
    parser.add_argument('-k', '--svk_nrows', default=1, type=positive_int)
    parser.add_argument('-t', '--progressive_steps', default=1, type=positive_int)

    args = parser.parse_args()

    version = args.version
    board = args.board
    tty_device = boards_info[board][0]
    flash_cmd = boards_info[board][1]

    svk_nrows = args.svk_nrows
    progressive_steps = args.progressive_steps

    if svk_nrows > n_polynomials[version]:
        parser.error(f'argument k: invalid choice: {svk_nrows} (can\'t be bigger than {n_polynomials[version]})')

    if progressive_steps > svk_nrows:
        parser.error(f'argument t: invalid choice: {progressive_steps} (can\'t be bigger than argument k)')

    return version, board, tty_device, flash_cmd, svk_nrows, progressive_steps
