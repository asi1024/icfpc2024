use std::backtrace;

use crate::icfp_lang::{Expr, UnaryOp, BinaryOp};

pub enum PPTree {
    Boolean(bool),
    Integer(String),
    String(String),
    Unary(UnaryOp, Box<PPTree>),
    Binary(BinaryOp, Vec<PPTree>),
    If(Box<PPTree>, Box<PPTree>, Box<PPTree>),
    Lambda(i64, Box<PPTree>),
    Variable(i64),
    Let(Vec<(i64, PPTree)>, Box<PPTree>),
}

impl PPTree {
    fn is_simple(&self) -> bool {
        match self {
            PPTree::Boolean(_) => true,
            PPTree::Integer(_) => true,
            PPTree::String(_) => true,
            PPTree::Variable(_) => true,
            _ => false,
        }
    }

    fn is_lambda(&self) -> bool {
        match self {
            PPTree::Lambda(_, _) => true,
            _ => false,
        }
    }

    fn destruct_lambda(self) -> (i64, PPTree) {
        match self {
            PPTree::Lambda(i, body) => (i, *body),
            _ => panic!("not a lambda"),
        }
    }

    fn is_let(&self) -> bool {
        match self {
            PPTree::Let(_, _) => true,
            _ => false,
        }
    }

    fn destruct_let(self) -> (Vec<(i64, PPTree)>, PPTree) {
        match self {
            PPTree::Let(bindings, body) => (bindings, *body),
            _ => panic!("not a let"),
        }
    }
}

fn build_pptree(expr: &Expr) -> PPTree {
    match expr {
        Expr::Boolean(b) => PPTree::Boolean(*b),
        Expr::Integer(i) => PPTree::Integer(i.to_string()),
        Expr::String(s) => PPTree::String(s.clone()),
        Expr::Neg(e) => PPTree::Unary(UnaryOp::Neg, Box::new(build_pptree(e))),
        Expr::Not(e) => PPTree::Unary(UnaryOp::Not, Box::new(build_pptree(e))),
        Expr::StrToInt(e) => PPTree::Unary(UnaryOp::StrToInt, Box::new(build_pptree(e))),
        Expr::IntToStr(e) => PPTree::Unary(UnaryOp::IntToStr, Box::new(build_pptree(e))),
        Expr::Add(e1, e2) => PPTree::Binary(BinaryOp::Add, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::Sub(e1, e2) => PPTree::Binary(BinaryOp::Sub, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::Mul(e1, e2) => PPTree::Binary(BinaryOp::Mul, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::Div(e1, e2) => PPTree::Binary(BinaryOp::Div, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::Mod(e1, e2) => PPTree::Binary(BinaryOp::Mod, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::Lt(e1, e2) => PPTree::Binary(BinaryOp::Lt, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::Gt(e1, e2) => PPTree::Binary(BinaryOp::Gt, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::Eq(e1, e2) => PPTree::Binary(BinaryOp::Eq, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::Or(e1, e2) => PPTree::Binary(BinaryOp::Or, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::And(e1, e2) => PPTree::Binary(BinaryOp::And, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::Concat(e1, e2) => PPTree::Binary(BinaryOp::Concat, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::TakeFirst(e1, e2) => PPTree::Binary(BinaryOp::TakeFirst, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::DropFirst(e1, e2) => PPTree::Binary(BinaryOp::DropFirst, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::Apply(e1, e2) => PPTree::Binary(BinaryOp::Apply, vec![build_pptree(e1), build_pptree(e2)]),
        Expr::If(e1, e2, e3) => PPTree::If(Box::new(build_pptree(e1)), Box::new(build_pptree(e2)), Box::new(build_pptree(e3))),
        Expr::Lambda(i, e) => PPTree::Lambda(*i, Box::new(build_pptree(e))),
        Expr::Variable(i) => PPTree::Variable(*i),
    }
}

fn convert_let(tree: &mut PPTree) {
    match tree {
        PPTree::Unary(_, child) => convert_let(child),
        PPTree::Binary(op, children) => {
            for child in children.iter_mut() {
                convert_let(child);
            }

            if *op == BinaryOp::Apply && children[0].is_lambda() {
                let mut ch = vec![];
                std::mem::swap(&mut ch, children);

                let (i, body) = ch.swap_remove(0).destruct_lambda();
                *tree = PPTree::Let(vec![(i, ch.swap_remove(0))], Box::new(body));
            }
        }
        PPTree::If(cond, then, els) => {
            convert_let(cond);
            convert_let(then);
            convert_let(els);
        }
        PPTree::Lambda(_, body) => convert_let(body),
        _ => return,
    }
}

fn flatten_binary(tree: &mut PPTree) {
    match tree {
        PPTree::Unary(_, child) => flatten_binary(child),
        PPTree::Binary(op, children) => {
            for child in children.iter_mut() {
                flatten_binary(child);
            }
            if !(*op == BinaryOp::Add || *op == BinaryOp::And) {
                return;
            }

            let mut cur_children = vec![];
            std::mem::swap(&mut cur_children, children);

            let mut new_children = vec![];
            for child in cur_children {
                match child {
                    PPTree::Binary(child_op, grand_children) if child_op == *op => {
                        new_children.extend(grand_children);
                    }
                    _ => new_children.push(child),
                }
            }
            *children = new_children;
        }
        PPTree::If(cond, then, els) => {
            flatten_binary(cond);
            flatten_binary(then);
            flatten_binary(els);
        }
        PPTree::Lambda(_, body) => flatten_binary(body),
        PPTree::Let(bindings, body) => {
            for (_, binding) in bindings.iter_mut() {
                flatten_binary(binding);
            }
            flatten_binary(body);

            if body.is_let() {
                let mut b = Box::new(PPTree::Boolean(false));
                std::mem::swap(&mut b, body);
                let (bindings2, body2) = b.destruct_let();
                bindings.extend(bindings2);
                *body = Box::new(body2);
            }
        }
        _ => return,
    }
}

fn spaces(depth: i32) -> String {
    " ".repeat(depth as usize * 2)
}

fn unary_op_name(op: UnaryOp) -> &'static str {
    match op {
        UnaryOp::Neg => "-",
        UnaryOp::Not => "!",
        UnaryOp::StrToInt => "StrToInt",
        UnaryOp::IntToStr => "IntToStr",
    }
}

fn binary_op_name(op: BinaryOp) -> &'static str {
    match op {
        BinaryOp::Add => "+",
        BinaryOp::Sub => "-",
        BinaryOp::Mul => "*",
        BinaryOp::Div => "/",
        BinaryOp::Mod => "%",
        BinaryOp::Lt => "<",
        BinaryOp::Gt => "<",
        BinaryOp::Eq => "==",
        BinaryOp::Or => "||",
        BinaryOp::And => "&&",
        BinaryOp::Concat => ".",
        BinaryOp::TakeFirst => "TakeFirst",
        BinaryOp::DropFirst => "DropFirst",
        BinaryOp::Apply => "Apply",
    }
}

fn dump_pptree(tree: &PPTree, depth: i32, buf: &mut Vec<String>) {
    match tree {
        PPTree::Boolean(b) => buf.push(format!("{}", b)),
        PPTree::Integer(i) => buf.push(format!("{}", i)),
        PPTree::String(s) => buf.push(format!("\"{}\"", s)),
        PPTree::Unary(op, child) => {
            buf.push(format!("({} ", unary_op_name(*op)));
            dump_pptree(child, depth, buf);
            buf.push(")".to_string());
        }
        PPTree::Binary(op, children) => {
            buf.push(format!("({}", binary_op_name(*op)));
            for child in children {
                buf.push("\n".to_string());
                buf.push(spaces(depth + 1));
                dump_pptree(child, depth + 1, buf);
            }
            buf.push("\n".to_string());
            buf.push(spaces(depth));
            buf.push(")".to_string());
        }
        PPTree::If(cond, then, els) => {
            buf.push("(If".to_string());
            buf.push("\n".to_string());
            buf.push(spaces(depth + 1));
            dump_pptree(cond, depth + 1, buf);
            buf.push("\n".to_string());
            buf.push(spaces(depth + 1));
            dump_pptree(then, depth + 1, buf);
            buf.push("\n".to_string());
            buf.push(spaces(depth + 1));
            dump_pptree(els, depth + 1, buf);
            buf.push("\n".to_string());
            buf.push(spaces(depth));
            buf.push(")".to_string());
        }
        PPTree::Lambda(i, body) => {
            buf.push(format!("(lambda v{}\n", i));
            buf.push(spaces(depth + 1));
            dump_pptree(body, depth + 1, buf);
            buf.push("\n".to_string());
            buf.push(spaces(depth));
            buf.push(")".to_string());
        }
        PPTree::Variable(i) => buf.push(format!("v{}", i)),
        PPTree::Let(bindings, body) => {
            buf.push("(let".to_string());
            for (i, binding) in bindings {
                buf.push("\n".to_string());
                buf.push(spaces(depth + 1));
                buf.push(format!("(v{} ", i));
                dump_pptree(binding, depth + 1, buf);
                buf.push(")".to_string());
            }
            buf.push("\n".to_string());
            buf.push(spaces(depth + 1));
            dump_pptree(body, depth + 1, buf);
            buf.push("\n".to_string());
            buf.push(spaces(depth));
            buf.push(")".to_string());
        }
    }
}

pub fn pretty_print(expr: &Expr) -> String {
    let mut tree = build_pptree(expr);
    convert_let(&mut tree);
    flatten_binary(&mut tree);
    let mut buf = vec![];
    dump_pptree(&tree, 0, &mut buf);
    buf.join("")
}
