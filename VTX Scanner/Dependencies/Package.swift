// swift-tools-version:4.0

import PackageDescription

let package = Package(
    name: "Dependencies",
    products: [
        .library(name: "Dependencies", type: .dynamic, targets: ["Dependencies"]),
    ],
    dependencies: [
      .package(url: "https://github.com/ReactiveCocoa/ReactiveSwift.git", from: "5.0.0"),
      .package(url: "https://github.com/deets/ReactiveCocoa.git", .revision("e6b0d920a80ffe6c4d69cfbadc8fa145377be0d3")),
      .package(url: "https://github.com/deets/BinUtils.git", .revision("c2c8e9777bc47d1d86e6b60fec79855e5b67c056")),
      .package(url: "https://github.com/deets/SwiftSVG.git", .exact("2.1.7")),
    ],
    targets: [
      .target(
        name: "Dependencies",
        dependencies: [
          "ReactiveSwift",
          "ReactiveCocoa",
          "BinUtils",
          "SwiftSVG",
        ],
        path: "."
      )
    ]
)
