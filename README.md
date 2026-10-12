# DJ Track Session Manager 🎧

A robust, multi-layered C++ systems programming application that simulates a professional DJ track session manager (inspired by hardware like Pioneer CDJ/DJM systems). The project emphasizes advanced C++ memory management, RAII principles, custom data structures, and a clean service-oriented architecture.

---

## 🚀 Key Features & Technical Highlights

- **Advanced Memory Management & Rule of 5:** 
  - Implemented deep copy and move semantics from scratch for the `AudioTrack` class hierarchy (`MP3Track`, `WAVTrack`) to safely manage dynamic waveform audio buffers without memory leaks.
  - Developed a custom smart pointer template wrapper (`PointerWrapper`) enforcing strict ownership transfer and RAII principles.
- **Polymorphic Architecture:** Utilizes virtual destructors, pure virtual interfaces, and polymorphic cloning to handle distinct audio formats seamlessly across system layers.
- **Multi-Service Architecture:** Clean separation of concerns divided into distinct layers:
  - **Library Service:** Parses configuration files and manages canonical track libraries and custom linked-list-based playlists.
  - **Controller Service (LRU Cache):** Implements a Least-Recently-Used (LRU) eviction cache using a fixed-capacity slot array to simulate hardware memory constraints.
  - **Mixing Engine Service:** Manages dual playback decks (Deck A / Deck B) featuring instant-transition policies, BPM compatibility checking, and automated tempo synchronization.
- **Rigorous Leak Prevention:** Fully tested and verified under Valgrind and AddressSanitizer for zero memory leaks or dangling pointers.

---

## 🛠️ System Architecture

The project is structured into modular layers mirroring real-world systems software:
1. **Infrastructure & Parsing:** Reads and validates configuration settings and playlist layouts (`ConfigurationManager`, `SessionFileParser`).
2. **Service Layer:** Houses the core business logic (`DJLibraryService`, `DJControllerService`, `MixingEngineService`).
3. **Cache Layer:** `LRUCache` coupled with `CacheSlot` and custom pointer wrappers.
4. **Orchestrator (`DJSession`):** Coordinates the main workflow loop in both interactive mode (`-I`) and automated batch mode (`-A`).

---

## ⚙️ Getting Started & Compilation

### Prerequisites
- A modern C++ compiler supporting C++11/C++14/C++17 (e.g., `g++`).
- Make build system.
- Valgrind (optional, for memory leak verification).

### Build Commands
Clone the repository and use the provided Makefile to build and test the project:

    # Compile in release mode
    make release

    # Compile with debug flags and run memory leak tests (Valgrind)
    make test-leaks

    # Clean compiled artifacts
    make clean

### Running the Application
To run the DJ session manager in interactive mode:

    ./bin/dj_manager -I

To run the application in automated mode (plays all playlists sequentially):

    ./bin/dj_manager -A

*(Ensure a valid configuration file `dj_config.txt` is present inside the `bin/` directory).*

---

## 📂 Project Structure

    ├── bin/                 # Executables and configuration files (dj_config.txt)
    ├── include/             # Header files (.h) defining interfaces and classes
    ├── src/                 # Implementation source files (.cpp)
    ├── Makefile             # Build automation rules
    └── README.md            # Project documentation