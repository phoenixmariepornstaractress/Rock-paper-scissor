#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <string>
#include <conio.h> // For getch()
#include <windows.h> // For Sleep()
#include <sstream> // For istringstream

enum Move { ROCK, PAPER, SCISSORS };

struct PlayerScore {
    std::string name;
    int score;
    int totalGames;
    int totalWins;

    bool operator<(const PlayerScore& other) const {
        return score > other.score; // Higher scores should come first
    }
};

struct Achievement {
    std::string description;
    bool unlocked;

    Achievement(const std::string& desc) : description(desc), unlocked(false) {}
};

struct PlayerProfile {
    std::string name;
    std::string password;
    PlayerScore score;
    std::vector<Achievement> achievements;
    std::vector<std::string> gameHistory;
    std::vector<std::string> friends;
    std::vector<std::string> messages;
};

struct GameState {
    std::vector<std::string> playerNames;
    int currentRound;
    int totalRounds;
    int currentPlayer;
    std::unordered_map<std::string, PlayerProfile> profiles;
};

Move getPlayerMove() {
    int choice;
    std::cout << "Enter your move (0 for ROCK, 1 for PAPER, 2 for SCISSORS): ";
    std::cin >> choice;

    while (std::cin.fail() || choice < 0 || choice > 2) {
        std::cin.clear(); // Clear the error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        std::cout << "Invalid input. Please enter 0, 1, or 2: ";
        std::cin >> choice;
    }

    return static_cast<Move>(choice);
}

Move getComputerMove() {
    return static_cast<Move>(std::rand() % 3);
}

void displayMove(Move move) {
    switch (move) {
        case ROCK:     std::cout << "ROCK"; break;
        case PAPER:    std::cout << "PAPER"; break;
        case SCISSORS: std::cout << "SCISSORS"; break;
    }
}

int determineWinner(Move playerMove, Move computerMove) {
    if (playerMove == computerMove) {
        std::cout << "It's a tie!" << std::endl;
        return 0; // tie
    } else if ((playerMove == ROCK && computerMove == SCISSORS) ||
               (playerMove == PAPER && computerMove == ROCK) ||
               (playerMove == SCISSORS && computerMove == PAPER)) {
        std::cout << "You win!" << std::endl;
        return 1; // player wins
    } else {
        std::cout << "Computer wins!" << std::endl;
        return -1; // computer wins
    }
}

int getNumberOfRounds() {
    int rounds;
    std::cout << "Enter the number of rounds you want to play: ";
    std::cin >> rounds;

    while (std::cin.fail() || rounds <= 0) {
        std::cin.clear(); // Clear the error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        std::cout << "Invalid input. Please enter a positive number: ";
        std::cin >> rounds;
    }

    return rounds;
}

void displayFinalResults(int playerWins, int computerWins, int ties) {
    std::cout << "\nFinal Results:" << std::endl;
    std::cout << "Player Wins: " << playerWins << std::endl;
    std::cout << "Computer Wins: " << computerWins << std::endl;
    std::cout << "Ties: " << ties << std::endl;

    if (playerWins > computerWins) {
        std::cout << "Congratulations! You are the overall winner!" << std::endl;
    } else if (computerWins > playerWins) {
        std::cout << "Computer is the overall winner. Better luck next time!" << std::endl;
    } else {
        std::cout << "It's a draw overall!" << std::endl;
    }
}

void saveScores(const std::unordered_map<std::string, PlayerProfile>& profiles) {
    std::ofstream file("leaderboard.txt");
    for (const auto& pair : profiles) {
        const auto& profile = pair.second;
        file << profile.name << " " << profile.password << " " << profile.score.score << " " << profile.score.totalGames << " " << profile.score.totalWins << std::endl;
        for (const auto& achievement : profile.achievements) {
            file << (achievement.unlocked ? "1" : "0") << " ";
        }
        file << std::endl;
        for (const auto& friendName : profile.friends) {
            file << friendName << " ";
        }
        file << std::endl;
        for (const auto& game : profile.gameHistory) {
            file << game << "|";
        }
        file << std::endl;
        for (const auto& message : profile.messages) {
            file << message << "|";
        }
        file << std::endl;
    }
}

std::unordered_map<std::string, PlayerProfile> loadScores() {
    std::unordered_map<std::string, PlayerProfile> profiles;
    std::ifstream file("leaderboard.txt");
    std::string line;

    while (std::getline(file, line)) {
        PlayerProfile profile;
        std::istringstream ss(line);
        ss >> profile.name >> profile.password >> profile.score.score >> profile.score.totalGames >> profile.score.totalWins;

        std::getline(file, line);
        std::istringstream achievementsStream(line);
        profile.achievements.emplace_back("Win 10 games");
        profile.achievements.emplace_back("Win 20 games");
        profile.achievements.emplace_back("Win 5 games in a row");
        for (auto& achievement : profile.achievements) {
            achievementsStream >> achievement.unlocked;
        }

        std::getline(file, line);
        std::istringstream friendsStream(line);
        std::string friendName;
        while (friendsStream >> friendName) {
            profile.friends.push_back(friendName);
        }

        std::getline(file, line);
        std::istringstream gameHistoryStream(line);
        std::string game;
        while (std::getline(gameHistoryStream, game, '|')) {
            profile.gameHistory.push_back(game);
        }

        std::getline(file, line);
        std::istringstream messagesStream(line);
        std::string message;
        while (std::getline(messagesStream, message, '|')) {
            profile.messages.push_back(message);
        }

        profiles[profile.name] = profile;
    }

    return profiles;
}

void displayLeaderboard(const std::unordered_map<std::string, PlayerProfile>& profiles) {
    std::vector<PlayerScore> scores;
    for (const auto& pair : profiles) {
        scores.push_back(pair.second.score);
    }
    std::sort(scores.begin(), scores.end());

    std::cout << "\nLeaderboard:\n";
    std::cout << std::setw(10) << "Name" << std::setw(10) << "Score" << std::setw(15) << "Total Games" << std::setw(15) << "Total Wins" << std::setw(15) << "Win Rate" << std::endl;
    for (const auto& score : scores) {
        double winRate = (score.totalGames > 0) ? (static_cast<double>(score.totalWins) / score.totalGames) * 100 : 0;
        std::cout << std::setw(10) << score.name << std::setw(10) << score.score << std::setw(15) << score.totalGames << std::setw(15) << score.totalWins << std::setw(14) << std::fixed << std::setprecision(2) << winRate << "%" << std::endl;
    }
}

void resetLeaderboard(std::unordered_map<std::string, PlayerProfile>& profiles) {
    profiles.clear();
    saveScores(profiles);
    std::cout << "Leaderboard has been reset." << std::endl;
}

void unlockAchievements(PlayerProfile& profile, int playerWins, int streak) {
    for (auto& achievement : profile.achievements) {
        if (!achievement.unlocked) {
            if (achievement.description == "Win 10 games" && playerWins >= 10) {
                achievement.unlocked = true;
                std::cout << "Achievement unlocked: " << achievement.description << std::endl;
            } else if (achievement.description == "Win 20 games" && playerWins >= 20) {
                achievement.unlocked = true;
                std::cout << "Achievement unlocked: " << achievement.description << std::endl;
            } else if (achievement.description == "Win 5 games in a row" && streak >= 5) {
                achievement.unlocked = true;
                std::cout << "Achievement unlocked: " << achievement.description << std::endl;
            }
        }
    }
}

void playGame(GameState& gameState) {
    std::string playerName = gameState.playerNames[gameState.currentPlayer];
    PlayerProfile& profile = gameState.profiles[playerName];

    int rounds = getNumberOfRounds();
    int playerWins = 0;
    int computerWins = 0;
    int ties = 0;
    int streak = 0;

    for (int round = 1; round <= rounds; ++round) {
        std::cout << "\nRound " << round << ":" << std::endl;

        Move playerMove = getPlayerMove();
        Move computerMove = getComputerMove();

        std::cout << "You played: ";
        displayMove(playerMove);
        std::cout << std::endl;

        std::cout << "Computer played: ";
        displayMove(computerMove);
        std::cout << std::endl;

        int result = determineWinner(playerMove, computerMove);
        if (result == 1) {
            ++playerWins;
            ++streak;
        } else if (result == -1) {
            ++computerWins;
            streak = 0;
        } else {
            ++ties;
        }
    }

    displayFinalResults(playerWins, computerWins, ties);

    profile.score.totalGames += rounds;
    profile.score.totalWins += playerWins;
    profile.score.score += (playerWins - computerWins); // Update the player's score
    unlockAchievements(profile, profile.score.totalWins, streak); // Check for new achievements

    std::ostringstream gameResult;
    gameResult << "Player Wins: " << playerWins << ", Computer Wins: " << computerWins << ", Ties: " << ties;
    profile.gameHistory.push_back(gameResult.str());

    gameState.currentPlayer = (gameState.currentPlayer + 1) % gameState.playerNames.size(); // Move to the next player
    saveScores(gameState.profiles); // Save updated profiles after each game
}

void displayUserProfile(const PlayerProfile& profile) {
    std::cout << "\nProfile for " << profile.name << ":\n";
    std::cout << "Total Games: " << profile.score.totalGames << "\n";
    std::cout << "Total Wins: " << profile.score.totalWins << "\n";
    std::cout << "Score: " << profile.score.score << "\n";

    std::cout << "Achievements:\n";
    for (const auto& achievement : profile.achievements) {
        std::cout << "- " << achievement.description << ": " << (achievement.unlocked ? "Unlocked" : "Locked") << "\n";
    }

    std::cout << "Friends:\n";
    for (const auto& friendName : profile.friends) {
        std::cout << "- " << friendName << "\n";
    }

    std::cout << "Game History:\n";
    for (const auto& game : profile.gameHistory) {
        std::cout << "- " << game << "\n";
    }

    std::cout << "Messages:\n";
    for (const auto& message : profile.messages) {
        std::cout << "- " << message << "\n";
    }
}

bool login(GameState& gameState, const std::string& name, const std::string& password) {
    auto it = gameState.profiles.find(name);
    if (it != gameState.profiles.end() && it->second.password == password) {
        gameState.playerNames.push_back(name);
        std::cout << "Login successful. Welcome, " << name << "!" << std::endl;
        return true;
    }
    std::cout << "Invalid name or password." << std::endl;
    return false;
}

bool registerUser(GameState& gameState, const std::string& name, const std::string& password) {
    if (gameState.profiles.find(name) == gameState.profiles.end()) {
        PlayerProfile profile;
        profile.name = name;
        profile.password = password;
        profile.score = { name, 0, 0, 0 };
        profile.achievements.emplace_back("Win 10 games");
        profile.achievements.emplace_back("Win 20 games");
        profile.achievements.emplace_back("Win 5 games in a row");
        gameState.profiles[name] = profile;
        gameState.playerNames.push_back(name);
        std::cout << "Registration successful. Welcome, " << name << "!" << std::endl;
        return true;
    }
    std::cout << "Name already taken. Please choose a different name." << std::endl;
    return false;
}

void addFriend(PlayerProfile& profile, const std::string& friendName) {
    if (std::find(profile.friends.begin(), profile.friends.end(), friendName) == profile.friends.end()) {
        profile.friends.push_back(friendName);
        std::cout << "Added " << friendName << " as a friend." << std::endl;
    } else {
        std::cout << friendName << " is already your friend." << std::endl;
    }
}

void sendMessage(PlayerProfile& profile, const std::string& recipientName, const std::string& message) {
    profile.messages.push_back("To " + recipientName + ": " + message);
    std::cout << "Message sent to " << recipientName << ": " << message << std::endl;
}

void viewMessages(const PlayerProfile& profile) {
    std::cout << "\nMessages:\n";
    for (const auto& message : profile.messages) {
        std::cout << message << std::endl;
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0))); // Seed the random number generator

    GameState gameState;
    gameState.profiles = loadScores();

    std::string choice;
    while (true) {
        std::cout << "\n1. Register\n2. Login\n3. Play Game\n4. View Profile\n5. View Leaderboard\n6. Reset Leaderboard\n7. Add Friend\n8. Send Message\n9. View Messages\n10. Exit\nEnter your choice: ";
        std::cin >> choice;

        if (choice == "1") {
            std::string name, password;
            std::cout << "Enter your name: ";
            std::cin >> name;
            std::cout << "Enter your password: ";
            std::cin >> password;
            registerUser(gameState, name, password);
        } else if (choice == "2") {
            std::string name, password;
            std::cout << "Enter your name: ";
            std::cin >> name;
            std::cout << "Enter your password: ";
            std::cin >> password;
            login(gameState, name, password);
        } else if (choice == "3") {
            if (gameState.playerNames.empty()) {
                std::cout << "Please register or login first." << std::endl;
            } else {
                playGame(gameState);
            }
        } else if (choice == "4") {
            std::string name;
            std::cout << "Enter your name: ";
            std::cin >> name;
            if (gameState.profiles.find(name) != gameState.profiles.end()) {
                displayUserProfile(gameState.profiles[name]);
            } else {
                std::cout << "Profile not found." << std::endl;
            }
        } else if (choice == "5") {
            displayLeaderboard(gameState.profiles);
        } else if (choice == "6") {
            resetLeaderboard(gameState.profiles);
        } else if (choice == "7") {
            std::string name, friendName;
            std::cout << "Enter your name: ";
            std::cin >> name;
            std::cout << "Enter your friend's name: ";
            std::cin >> friendName;
            if (gameState.profiles.find(name) != gameState.profiles.end()) {
                addFriend(gameState.profiles[name], friendName);
            } else {
                std::cout << "Profile not found." << std::endl;
            }
        } else if (choice == "8") {
            std::string name, recipientName, message;
            std::cout << "Enter your name: ";
            std::cin >> name;
            std::cout << "Enter the recipient's name: ";
            std::cin >> recipientName;
            std::cin.ignore(); // Ignore the newline character left by std::cin
            std::cout << "Enter your message: ";
            std::getline(std::cin, message);
            if (gameState.profiles.find(name) != gameState.profiles.end()) {
                sendMessage(gameState.profiles[name], recipientName, message);
            } else {
                std::cout << "Profile not found." << std::endl;
            }
        } else if (choice == "9") {
            std::string name;
            std::cout << "Enter your name: ";
            std::cin >> name;
            if (gameState.profiles.find(name) != gameState.profiles.end()) {
                viewMessages(gameState.profiles[name]);
            } else {
                std::cout << "Profile not found." << std::endl;
            }
        } else if (choice == "10") {
            break;
        } else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}
