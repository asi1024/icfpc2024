#%%
import argparse
import sys
import subprocess

CONVERT_MAP = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`|~ \n"
INVERT_MAP = [None] * 128
for i, c in enumerate(CONVERT_MAP):
    INVERT_MAP[ord(c)] = chr(i + 33)


def from_icfp(s: str) -> str:
    res = subprocess.run("./icfp_parser/target/release/run", shell=True, text=s, input=s, check=True, stdout=subprocess.PIPE).stdout
    x = res.strip()
    assert x[:8] == 'String("'
    assert x[-2:] == '")'
    x = x[8:]
    x = x[:-2]
    x = x.replace("\\n", "\n")
    return x
    # return "".join(CONVERT_MAP[c - 33] for c in b)


def to_icfp(s: str) -> str:
    b = s.encode("ascii")
    return "S" + "".join(INVERT_MAP[c] for c in b)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--to-icfp", action="store_true")
    args = parser.parse_args()

    if args.to_icfp:
        text = sys.stdin.read()
        print(to_icfp(text), end="")
    else:
        text = input()
        print(from_icfp(text), end="")


if __name__ == "__main__":
    main()
