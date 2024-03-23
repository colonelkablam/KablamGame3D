#pragma once
class Command {
public:
    virtual ~Command() {}
    virtual void execute() = 0; // Apply changes to the canvas
    virtual void undo() = 0;    // Revert changes from the canvas
};

