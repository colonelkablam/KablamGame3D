#pragma once

class ICommand {
public:
    virtual ~ICommand() {}
    virtual void execute() = 0; // Apply changes to the canvas
    virtual void undo() = 0;    // Revert changes from the canvas
};

