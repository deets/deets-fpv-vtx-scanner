// swift-tools-version:4.0

import PackageDescription

let package = Package(
    name: "Dependencies",
    products: [
        .library(name: "Dependencies", type: .dynamic, targets: ["Dependencies"]),
    ],
    dependencies: [
      .package(url: "https://github.com/ReactiveCocoa/ReactiveSwift.git", from: "3.0.0"),
      .package(url: "https://github.com/nst/BinUtils.git", from: "0.1.0")
    ],
    targets: [
        .target(name: "Dependencies", dependencies: ["ReactiveSwift", "BinUtils"], path: "." )
    ]
)
