def wordlist_to_int(wordlist, word_size, endianess = 'big'):
    value = 0

    if endianess == 'little':
        for i in range(len(wordlist)):
            value += wordlist[i] * 2**(word_size * i)
    else:
        for i in range(len(wordlist)):
            value += wordlist[i] * 2**(word_size * (len(wordlist) - i - 1))

    return value

def wordlist_to_int_v2(wordlist, word_size, endianess = 'big'):
    value = 0

    if endianess == 'little':
        for i in range(len(wordlist)):
            value |= wordlist[i] << (word_size * i)
    else:
        for i in range(len(wordlist)):
            value |= wordlist[i] << (word_size * (len(wordlist) - i - 1))

    return value

if __name__ == '__main__':
    for i in range(1000):
        pass