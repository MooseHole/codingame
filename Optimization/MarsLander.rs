use std::io;

macro_rules! parse_input {
    ($x:expr, $t:ident) => ($x.trim().parse::<$t>().unwrap())
}

/**
 * Save the Planet.
 * Use less Fossil Fuel.
 **/
fn main() {
    let mut input_line = String::new();
    io::stdin().read_line(&mut input_line).unwrap();
    let n = parse_input!(input_line, i32); // the number of points used to draw the surface of Mars.
    for i in 0..n as usize {
        let mut input_line = String::new();
        io::stdin().read_line(&mut input_line).unwrap();
        let inputs = input_line.split(" ").collect::<Vec<_>>();
        let land_x = parse_input!(inputs[0], i32); // X coordinate of a surface point. (0 to 6999)
        let land_y = parse_input!(inputs[1], i32); // Y coordinate of a surface point. By linking all the points together in a sequential fashion, you form the surface of Mars.
    }

    // game loop
    loop {
        let mut input_line = String::new();
        io::stdin().read_line(&mut input_line).unwrap();
        let inputs = input_line.split(" ").collect::<Vec<_>>();
        let x = parse_input!(inputs[0], i32);
        let y = parse_input!(inputs[1], i32);
        let hs = parse_input!(inputs[2], i32); // the horizontal speed (in m/s), can be negative.
        let vs = parse_input!(inputs[3], i32); // the vertical speed (in m/s), can be negative.
        let f = parse_input!(inputs[4], i32); // the quantity of remaining fuel in liters.
        let r = parse_input!(inputs[5], i32); // the rotation angle in degrees (-90 to 90).
        let p = parse_input!(inputs[6], i32); // the thrust power (0 to 4).

        // Write an action using println!("message...");
        // To debug: eprintln!("Debug message...");


        // R P. R is the desired rotation angle. P is the desired thrust power.
        println!("-20 3");
    }
}
