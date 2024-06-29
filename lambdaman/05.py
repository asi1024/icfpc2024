from convert_string import to_icfp


problem = """.....########...
....#...........
...#..######....
..#..#......#...
.#..#...##...#..
.#..#..#L.#...#.
.#...#....#...#.
..#...####...#..
...#........#...
....########....
................""".split("\n")
problem = [list(row) for row in problem]


height = len(problem)
width = len(problem[0])

for y in range(height):
    for x in range(width):
        if problem[y][x] == "L":
            py = y
            px = x
            break


cmd = "RDLLLULURRULRRRRRDLRRDRDLLUDDRLLDRLLLLLLLURLLURLLURULRURURURRRRRRRDRDRUULURRDDDDLRDDDLDRDDLULUDDLUDLLLLLLLLLULDLLURURLULDUUUUURULUURDRDUURDUR"

for c in cmd:
    if c == "U":
        py -= 1
    elif c == "D":
        py += 1
    elif c == "L":
        px -= 1
    elif c == "R":
        px += 1

    if problem[py][px] == "#":
        assert False
    problem[py][px] = "L"


for row in problem:
    print("".join(row))
    assert "." not in row


code = f"solve lambdaman5 {cmd}"
print(to_icfp(code))
