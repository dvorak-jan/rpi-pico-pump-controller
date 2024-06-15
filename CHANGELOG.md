# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [unreleased]

### Added
- When the water is at elevated level (not exceeding the high threshold) for a certain amount of time, it is pumped out

### Changed
- Include pico-sdk as submodule
- Include FreeRTOS-Kernel as submodule


## [0.1.0] - 2024-06-01

### Added
- Control LEDs
- Read analog inputs from the hydrostatic sensor
- Switch on the relay when the level exceeds defined level
- Switch off the relay when the level decreases under defined level
