use std::env;

use image::Luma;

const TARGET_WIDTH: u32 = 800;
const TARGET_HEIGHT: u32 = 480;

fn main() {
    let args: Vec<String> = env::args().collect();

    let source = &args[1];
    let target = &args[2];

    process_image(source, target);
}

fn process_image(source: &str, target: &str) {
    let source_image = image::open(source)
        .unwrap()
        .resize_to_fill(
            TARGET_WIDTH,
            TARGET_HEIGHT,
            image::imageops::FilterType::Gaussian,
        )
        .to_luma8();
    let mut target_image = image::GrayImage::new(TARGET_WIDTH, TARGET_HEIGHT);

    dither(&source_image, &mut target_image);

    println!("SAVING");
    target_image.save(target).unwrap();
    println!("SAVED {:?}", target);
}

fn get_lumen(luma: &image::Luma<u8>) -> u8 {
    luma[0]
}

fn dither(
    source: &image::ImageBuffer<Luma<u8>, Vec<u8>>,
    target: &mut image::ImageBuffer<Luma<u8>, Vec<u8>>,
) {
    let mut errors = vec![0; (TARGET_WIDTH * TARGET_HEIGHT) as usize];

    for y in 0..TARGET_HEIGHT {
        for x in 0..TARGET_WIDTH {
            let error = errors.get((TARGET_WIDTH * y + x) as usize).unwrap();
            let source_pixel = source.get_pixel(x, y);
            let lumen = get_lumen(source_pixel) as i16 + error;

            let t = 127;
            let dark = lumen < t;
            let new_error = if dark { lumen } else { lumen - 255 };

            let mut write_error = |i: usize, z| {
                if let Some(e) = errors.get_mut(i) {
                    *e += z;
                }
            };

            let line = TARGET_WIDTH as usize;
            let index = line * y as usize + x as usize;

            // Atkinson
            write_error(index + 1, new_error / 8);
            write_error(index + 2, new_error / 8);
            write_error(index + line - 1, new_error / 8);
            write_error(index + line, new_error / 8);
            write_error(index + line + 1, new_error / 8);
            write_error(index + 2 * line, new_error / 8);

            // Floyd Steinberg
            // write_error(index + 1, new_error / 16 * 7);
            // write_error(index + line - 1, new_error / 16 * 3);
            // write_error(index + line, new_error / 16 * 5);
            // write_error(index + line + 1, new_error / 16);

            target.put_pixel(
                x,
                y,
                if dark {
                    Luma([u8::MIN])
                } else {
                    Luma([u8::MAX])
                },
            );
        }
    }
}
