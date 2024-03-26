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

    void PerformAction(Command* command) {
        command->execute();
        undoStack.push(command);
        // Clear the redoStack
        while (!redoStack.empty()) {
            Command* commandToBeDeleted = redoStack.top(); // Get the top command from redoStack
            redoStack.pop();                               // Remove the top element from redoStack
            delete commandToBeDeleted;                     // Delete the command object
        }
    }

    void Undo() {
        if (undoStack.empty() == false) {
            Command* command = undoStack.top();
            undoStack.pop();
            command->undo();
            redoStack.push(command);
        }
    }

    void Redo() {
        if (!redoStack.empty()) {
            Command* command = redoStack.top();
            redoStack.pop();
            command->execute();
            undoStack.push(command);
        }
    }

    int GetUndoStackSize()
    {
        return undoStack.size();
    }

    int GetRedoStackSize()
    {
        return redoStack.size();
    }
};
