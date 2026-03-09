FROM ubuntu:24.04 AS build

RUN apt-get update && \
    apt-get install -y g++ && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY main.cpp .
RUN g++ -O2 -std=c++17 -o hello-web main.cpp

FROM ubuntu:24.04

WORKDIR /app
COPY --from=build /src/hello-web .

EXPOSE 8080
CMD ["./hello-web"]