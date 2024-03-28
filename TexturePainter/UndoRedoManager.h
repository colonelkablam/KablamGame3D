#pragma once

#include <stack>

#include "ICommand.h"

class UndoRedoManager {
    std::stack<ICommand*> undoStack;
    std::stack<ICommand*> redoStack;

public:
    ~UndoRedoManager() {
        while (!undoStack.empty()) {
            ICommand* com = undoStack.top(); // Get the top command pointer
            undoStack.pop();               // Remove the top element from the stack
            delete com;                    // Delete the command object
        }

        // Repeat the same for redoStack
        while (!redoStack.empty()) {
            ICommand* com = redoStack.top(); // Get the top command pointer
            redoStack.pop();                // Remove the top element from the stack
            delete com;                     // Delete the command object
        }
    }

    void PerformAction(ICommand* command) {
        command->execute();
        undoStack.push(command);
        // Clear the redoStack
        while (!redoStack.empty()) {
            ICommand* commandToBeDeleted = redoStack.top(); // Get the top command from redoStack
            redoStack.pop();                               // Remove the top element from redoStack
            delete commandToBeDeleted;                     // Delete the command object
        }
    }

    void Undo() {
        if (undoStack.empty() == false) {
            ICommand* command = undoStack.top();
            undoStack.pop();
            command->undo();
            redoStack.push(command);
        }
    }

    void Redo() {
        if (!redoStack.empty()) {
            ICommand* command = redoStack.top();
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
