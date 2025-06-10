# Trivia Game

A multi-player trivia game application with a C++ server backend and a WPF client frontend. This application allows users to create and join game rooms, play trivia games with friends, and compete in head-to-head matches.

## Features

- **User Authentication**: Secure login and registration system
- **Lobby System**: Create and join game rooms
- **Multiplayer Gameplay**: Compete with others in real-time trivia challenges
- **Head-to-Head Mode**: Quick 1v1 matches with automatic opponent matching
- **Question Database**: Growing collection of trivia questions
- **User Contributions**: Add your own questions to the database
- **Real-time Results**: Live scoring and game statistics
- **Leaderboards**: See who's the trivia champion

## Technologies Used

- **Backend**: 
  - C++ server with multithreading
  - SQLite database for user accounts and questions
  - Custom networking protocol for client-server communication
  - JSON serialization for data exchange

- **Frontend**:
  - C# WPF application
  - MVVM architecture
  - Modern UI design with responsive layout
  - Asynchronous communication with server

## Architecture

The application follows a client-server architecture:

- **Server**: Handles authentication, room management, game logic, and database operations
- **Client**: Provides the user interface and communicates with the server via socket connections

## Setup and Installation

### Prerequisites

- Visual Studio 2019 or newer with C++ and C# development tools
- .NET Framework 4.7.2 or higher
- SQLite

### Server Setup

1. Open `Trivia.sln` in Visual Studio
2. Build the `Trivia` project
3. Run the server executable

### Client Setup

1. Open `Trivia.sln` in Visual Studio
2. Build the `client_gui` project
3. Run the client application

## How to Play

1. Launch the client application and log in or register
2. From the main menu, you can:
   - Join an existing game room
   - Create a new game room
   - Challenge someone to a head-to-head match
   - Add new questions to the database
3. In a game room, wait for the host to start the game
4. Answer trivia questions within the time limit
5. View results at the end of the game to see who won

## Future Improvements

- Mobile client applications
- More game modes (team play, categories, difficulty levels)
- User profiles and statistics
- Achievement system
- Integration with online trivia APIs for additional questions