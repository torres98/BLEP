from hashlib import sha256, sha384, sha512
import numpy as np

from .gf.gf16 import gf16
from .gf.gf256 import gf256


def __rainbow_generator(version):
    if version == 1:
        v1, o1, o2 = 36, 32, 32
        element_hex_size = 1
        DIGEST_SIZE = 32

        hash_function = sha256
        gf = gf16
        
    elif version == 2:
        v1, o1, o2 = 68, 32, 48
        element_hex_size = 2
        DIGEST_SIZE = 48

        hash_function = sha384
        gf = gf256

    elif version == 3:
        v1, o1, o2 = 96, 36, 64
        element_hex_size = 2
        DIGEST_SIZE = 64

        hash_function = sha512
        gf = gf256
        
    else:
        raise ValueError(f'Non-existing version ({version})')

    n_variables = v1 + o1 + o2
    n_polynomials = n_variables - v1
    N = (n_variables**2 + n_variables) // 2

    remaining_bytes = n_polynomials - DIGEST_SIZE * (2 // element_hex_size)

    def parse_public_key(pk_path):
        """Returns the matrix for the Mv-style verification from the rainbow public key file."""

        f = open(pk_path, 'r')
        pk_str = f.read()
        f.close()

        pk_str_offset = pk_str.index('=') + 2

        M = np.empty((n_polynomials, N + n_polynomials), dtype=object)
        
        for i in range(N):
            for j in range(n_polynomials):
                elem_str = pk_str[pk_str_offset:pk_str_offset+element_hex_size]
                M[j, i] = gf(int(elem_str, 16))    
                pk_str_offset += element_hex_size

        #setup public key matrix for Mv verification
        for i in range(n_polynomials):
            for j in range(N, N + n_polynomials):
                M[i, j] = gf(0)

            M[i, N + i] = gf(1)
        
        return M

    def parse_signature(signature_path):
        """Returns the signature vector and the salt for the message digest stored in the signature file"""
        f = open(signature_path, 'r')
        sign_str = f.read().rstrip()
        f.close()

        z_start = sign_str.index('=') + 2
        salt_start = z_start + n_variables * element_hex_size

        # extract the map input z (variable values)
        z = [gf(int(sign_str[i:i+element_hex_size], 16)) for i in range(z_start, salt_start, element_hex_size)]
        
        if element_hex_size == 1:
            for i in range(0, n_variables, 2):
                z[i+1], z[i] = z[i], z[i+1]

        # compute the quadratic vector s
        s = np.array([z[i] * z[j] for i in range(n_variables) for j in range(i, n_variables)], dtype=object)

        # extract the salt value
        salt = bytes.fromhex(sign_str[salt_start:])

        return s, salt

    def message_digest(message_path, salt):
        f = open(message_path, 'rb')
        message = f.read()
        f.close()

        message_digest = hash_function(message).digest()

        message_and_salt_digest = hash_function(message_digest + salt).digest()

        if remaining_bytes == 0:
            return message_and_salt_digest.hex()
        else: #fill the remaining bytes with another hash
            return (message_and_salt_digest + hash_function(message_and_salt_digest).digest()[:remaining_bytes]).hex()

    def get_signature_vector(s, message_digest):
        h = np.array([gf(int(message_digest[i:i+element_hex_size], 16)) for i in range(0, len(message_digest), element_hex_size)], dtype=object)

        return np.concatenate((s, h), dtype=object)

    import types
    rainbow_module = types.ModuleType(f'Rainbow{version}', '...')
    rainbow_module.__dict__.update({
        'parse_public_key': parse_public_key,
        'parse_signature': parse_signature,
        'message_digest': message_digest,
        'get_signature_vector': get_signature_vector
    })

    return rainbow_module

RainbowI = __rainbow_generator(1)
RainbowIII = __rainbow_generator(2)
RainbowV = __rainbow_generator(3)

def __rainbow_generator(version):
    if version == 1:
        v1, o1, o2 = 36, 32, 32
        element_hex_size = 1
        DIGEST_SIZE = 32

        hash_function = sha256
        gf = gf16
        
    elif version == 2:
        v1, o1, o2 = 68, 32, 48
        element_hex_size = 2
        DIGEST_SIZE = 48

        hash_function = sha384
        gf = gf256

    elif version == 3:
        v1, o1, o2 = 96, 36, 64
        element_hex_size = 2
        DIGEST_SIZE = 64

        hash_function = sha512
        gf = gf256
        
    else:
        raise ValueError(f'Non-existing version ({version})')

    n_variables = v1 + o1 + o2
    n_polynomials = n_variables - v1
    N = (n_variables**2 + n_variables) // 2

    remaining_bytes = n_polynomials - DIGEST_SIZE * (2 // element_hex_size)

    def parse_public_key(pk_path):
        """Returns the matrix for the Mv-style verification from the rainbow public key file."""

        f = open(pk_path, 'r')
        pk_str = f.read()
        f.close()

        pk_str_offset = pk_str.index('=') + 2

        PK = np.empty((n_polynomials, N), dtype=object)
        
        for i in range(N):
            for j in range(n_polynomials):
                elem_str = pk_str[pk_str_offset:pk_str_offset+element_hex_size]
                PK[j, i] = gf(int(elem_str, 16))    
                pk_str_offset += element_hex_size
        
        return PK

    def parse_signature(signature_path):
        """Returns the signature vector and the salt for the message digest stored in the signature file"""
        f = open(signature_path, 'r')
        sign_str = f.read().rstrip()
        f.close()

        z_start = sign_str.index('=') + 2
        salt_start = z_start + n_variables * element_hex_size

        # extract the map input z (variable values)
        z = [gf(int(sign_str[i:i+element_hex_size], 16)) for i in range(z_start, salt_start, element_hex_size)]
        
        if element_hex_size == 1:
            for i in range(0, n_variables, 2):
                z[i+1], z[i] = z[i], z[i+1]

        # compute the quadratic vector s
        s = np.array([z[i] * z[j] for i in range(n_variables) for j in range(i, n_variables)], dtype=object)

        # extract the salt value
        salt = bytes.fromhex(sign_str[salt_start:])

        return s, salt

    def message_digest(message_path, salt):
        f = open(message_path, 'rb')
        message = f.read()
        f.close()

        message_digest = hash_function(message).digest()

        message_and_salt_digest = hash_function(message_digest + salt).digest()

        if remaining_bytes == 0:
            return message_and_salt_digest.hex()
        else: #fill the remaining bytes with another hash
            padding_digest = hash_function(message_and_salt_digest).digest()
            return (message_and_salt_digest + padding_digest)[:n_polynomials * (2 // element_hex_size)].hex()

    def get_result_vector(message_digest):
        return np.array([gf(int(message_digest[i:i+element_hex_size], 16)) for i in range(0, len(message_digest), element_hex_size)], dtype=object)

    import types
    rainbow_module = types.ModuleType(f'Rainbow{version}', '...')
    rainbow_module.__dict__.update(locals())

    return rainbow_module

RainbowI = __rainbow_generator(1)
RainbowIII = __rainbow_generator(2)
RainbowV = __rainbow_generator(3)
