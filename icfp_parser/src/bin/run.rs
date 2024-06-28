use icfp_parser::icfp_lang;

fn main() {
    // read a line from stdin
    let mut input = String::new();
    std::io::stdin().read_line(&mut input).unwrap();

    // parse the input
    let toks = icfp_lang::parse_program(&input);
    let expr = icfp_lang::parse_tokens(&toks);
    let result = icfp_lang::evaluate(&expr);
    println!("{:?}", result);
}
