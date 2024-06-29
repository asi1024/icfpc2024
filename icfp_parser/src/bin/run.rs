use std::io::Read;
use icfp_parser::icfp_lang;

fn main() {
    // read the entire stdin
    let mut input = String::new();
    std::io::stdin().read_to_string(&mut input).unwrap();

    // parse the input
    let toks = icfp_lang::parse_program(&input);
    let expr = icfp_lang::parse_tokens(&toks);
    let result = icfp_lang::evaluate(&expr);
    println!("{:?}", result);
}
