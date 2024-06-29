use icfp_parser::icfp_lang::{parse_program, parse_tokens, Expr, UnaryOp, BinaryOp};
use icfp_parser::pretty_print::{PPTree, flatten_binary, build_pptree};

fn extract_if(expr: Expr) -> Option<Expr> {
    match expr {
        Expr::If(cond, then, els) => Some(*cond),
        Expr::Lambda(_, e) => extract_if(*e),
        Expr::Apply(f, a) => {
            let f = extract_if(*f);
            let a = extract_if(*a);
            f.or(a)
        }
        _ => None,
    }
}

fn dump_cspuz(expr: Expr) {
    match expr {
        Expr::Boolean(true) => print!("True"),
        Expr::Boolean(false) => print!("False"),
        Expr::And(e1, e2) => {
            print!("(");
            dump_cspuz(*e1);
            print!(" & ");
            dump_cspuz(*e2);
            print!(")");
        }
        Expr::Or(e1, e2) => {
            print!("(");
            dump_cspuz(*e1);
            print!(" | ");
            dump_cspuz(*e2);
            print!(")");
        }
        Expr::Not(e) => {
            print!("(~");
            dump_cspuz(*e);
            print!(")");
        }
        Expr::Eq(e1, e2) => {
            print!("(");
            dump_cspuz(*e1);
            print!(" == ");
            dump_cspuz(*e2);
            print!(")");
        }
        Expr::Variable(v) => print!("v[{}]", v),
        _ => todo!(),
    }
}


fn dump_cspuz_pptree(expr: PPTree) {
    match expr {
        PPTree::Boolean(true) => print!("True"),
        PPTree::Boolean(false) => print!("False"),
        PPTree::Unary(op, e) => {
            print!("(");
            match op {
                UnaryOp::Not => print!("~"),
                UnaryOp::Neg => print!("-"),
                _ => panic!(),
            }
            dump_cspuz_pptree(*e);
            print!(")");
        }
        PPTree::Variable(v) => print!("v[{}]", v),
        PPTree::Integer(i) => print!("{}", i),
        PPTree::Binary(BinaryOp::Eq, mut operands) => {
            let op1 = operands.pop().unwrap();
            let op0 = operands.pop().unwrap();
            print!("(");
            dump_cspuz_pptree(op0);
            print!(" == ");
            dump_cspuz_pptree(op1);
            print!(")");
        }
        PPTree::Binary(BinaryOp::Or, mut operands) => {
            let op1 = operands.pop().unwrap();
            let op0 = operands.pop().unwrap();
            print!("(");
            dump_cspuz_pptree(op0);
            print!(" | ");
            dump_cspuz_pptree(op1);
            print!(")");
        }
        PPTree::Binary(BinaryOp::And, operands) => {
            print!("fold_and(");
            for (i, op) in operands.into_iter().enumerate() {
                if i > 0 {
                    print!(", ");
                }
                dump_cspuz_pptree(op);
            }
            print!(")");
        }
        _ => panic!(),
    }
}

fn main() {
    // read a line from stdin
    let mut input = String::new();
    std::io::stdin().read_line(&mut input).unwrap();

    // parse the input
    let toks = parse_program(&input);
    let expr = parse_tokens(&toks);
    let expr_if = extract_if(expr);
    assert!(expr_if.is_some());

    let mut pptree = build_pptree(&expr_if.unwrap());
    flatten_binary(&mut pptree);

    dump_cspuz_pptree(pptree);
    println!();
}
