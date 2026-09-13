# Smart Flashcard Vocabulary Trainer (C++20 & Python)

An advanced, logistically planned vocabulary training application based on the scientific **Leitner 6-box flashcard system**, designed to be paired with an automated Python data-ingestion pipeline.

> **Status: Active Development / Major Refactoring Phase (WIP)**  
> *Note: The core application logic and UI are currently implemented in German. The project is undergoing a structural overhaul to fix broken features and optimize the architecture.*

## 🚀 Key Features (Current Implementation)

* **Leitner 6-Box System:** Implements a spaced-repetition framework with 6 sequential boxes to optimize vocabulary retention.
* **Rich Data Processing:** Parses structured vocabulary entries consisting of: *Target Word | Translation | Hint/Clue | Current Box (1-6) | Contextual Example Sentence*.
* **Interactive Learning Modes:**
  * *Standard Box Review:* Learning by selecting specific Leitner boxes (1 to 6).
  * *Blitz Mode:* A rapid-fire review session for quick, randomized vocabulary checks.
* **Smart Assist Features:** Provides dynamic hints during a session and displays contextual example sentences upon correct answers to reinforce learning.

## 🛠️ Tech Stack & Current Architecture

* **Core Application:** C++20 (Developed and tested using CLion)
* **Data Layer:** File-based data storage parsing structured text strings containing definitions, hints, and example sentences.
* **External Tooling:** Experimental Python script designed for AI prompting and data formatting.

## 🚧 Current Major Challenges & Roadmap (Refactoring Goals)

The project is currently in a critical refactoring phase to transition from a monolithic script into a scalable application. The main objectives are:

- [ ] **Architectural Separation (UI vs. Logic):** Redesigning the core structure by applying the multi-layer architecture used in my *Household Planner* project (separating into strict UI Presentation and Core Engine layers).
- [ ] **API Pipeline Repair:** Debugging and fixing the integration of the Python script to successfully download, translate, and pipe dynamic AI-generated vocabulary data from the web into the C++ engine.
- [ ] **Linguistic Categorization:** Overhauling the learning system to allow filtering and training by **specific topics/themes** instead of blindly grinding through entire Leitner boxes.
