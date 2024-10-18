FROM rust:1.81 as builder

# 2. Copy the files in your machine to the Docker image
COPY ./ditherer /opt/ditherer-build
WORKDIR /opt/ditherer-build
RUN cargo build --release

FROM fedora-minimal:40
COPY --from=builder /opt/ditherer-build/target/release/ditherer /usr/bin
RUN microdnf install -y gdal
COPY naturalearthdata_boundary_lines_central_europe /opt/clouds/naturalearthdata_boundary_lines_central_europe
COPY hack /opt/clouds/hack
RUN mkdir /opt/clouds/eumetsat_mtg_geocolour_rgb_central_europe
