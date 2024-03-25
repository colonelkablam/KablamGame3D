#pragma once

#include <stack>

#include "Command.h"

class UndoRedoManager {
    std::stack<Command*> undoStack;
    std::stack<Command*> redoStack;

public:
    ~UndoRedoManager() {
        while (!undoStack.empty()) {
            Command* com = undoStack.top(); // Get the top command pointer
            undoStack.pop();               // Remove the top element from the stack
            delete com;                    // Delete the command object
        }

        // Repeat the same for redoStack
        while (!redoStack.empty()) {
            Command* com = redoStack.top(); // Get the top command pointer
            redoStack.pop();                // Remove the top element from the stack
            delete com;                     // Delete the command object
        }
    }

    void performAction(Command* command) {
        command->execute();
        undoStack.push(command);
        while (!redoStack.empty()) redoStack.pop(); // Clear the redoStack
    }

    void undo() {
        if (!undoStack.empty()) {
            Command* command = undoStack.top();
            undoStack.pop();
            command->undo();
            redoStack.push(command);
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            Command* command = redoStack.top();
            redoStack.pop();
            command->execute();
            undoStack.push(command);
        }
    }
};
