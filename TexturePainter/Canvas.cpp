#include <Windows.h>

// undefine the macros from windows.h
#undef max
#undef min

// need to include full header here to use class methods from DI
#include "TexturePainter.h"
#include "Canvas.h"
#include "BrushstrokeCommand.h"
#include "IToolState.h"
#include "ConcreteToolStates.h" // Contains definitions for BlockBrushState, RectBrushState, etc.
#include "CanvasCoordinateStrategy.h"

// define static members
// tool icons
Texture* Canvas::deleteToolIcon = nullptr;
Texture* Canvas::blockToolIcon = nullptr;
Texture* Canvas::increaseToolIcon = nullptr;
Texture* Canvas::decreaseToolIcon = nullptr;
Texture* Canvas::lineToolIcon = nullptr;
Texture* Canvas::rectToolIcon = nullptr;
Texture* Canvas::rectFillToolIcon = nullptr;
Texture* Canvas::circleToolIcon = nullptr;
Texture* Canvas::circleFillToolIcon = nullptr;
Texture* Canvas::clipboardToolIcon = nullptr;
Texture* Canvas::clipboardToolToggleIcon = nullptr;
Texture* Canvas::clipboardToolToggle2Icon = nullptr;
Texture* Canvas::sharedClipboardSaveIcon = nullptr;
Texture* Canvas::sharedClipboardDeleteIcon = nullptr;
Texture* Canvas::sharedClipboardLoadIcon = nullptr;
Texture* Canvas::sharedClipboardLoadedIcon = nullptr;
bool Canvas::sharedClipboardFilled = false;


// shared clipboard initialised with default nullptr
Canvas::TextureSample* Canvas::sharedClipboardTextureSample{ nullptr };

// Primary constructor that does all the work
Canvas::Canvas(TexturePainter& drawer, int width, int height, int illumination, const std::wstring& saveFolder, const std::wstring& fileName, short xPos, short yPos)
    : drawingClass{ drawer },
    backgroundTexture{ width, height, illumination },
    clipboardTextureSample{ nullptr }, // clipboard initialised with default nullptr
    paintPartialSample{ false },
    topLeft{ xPos, yPos },
    canvasViewOffset{ 0, 0 },
    zoomLevel{ START_ZOOM_LEVEL },
    currentToolState{ nullptr },
    coordinateStrategy{std::move(std::make_unique<CanvasCoordinateStrategy>(topLeft, canvasViewOffset, zoomLevel))}
{
    Initialise(saveFolder, fileName);
    PostInitialise();
}

// Delegating constructor for texture loading from file
Canvas::Canvas(TexturePainter& drawer, const std::wstring& saveFolder, const std::wstring& fileName, short xPos, short yPos)
    : Canvas(drawer, 0, 0, 0, saveFolder, fileName, xPos, yPos) // Delegate to the primary constructor
{
    std::wstring fullPath = saveFolder + fileName;
    // Attempt to load the texture;
    if (!backgroundTexture.LoadFrom(fullPath)) {
        // Fallback initialization if loading fails
        backgroundTexture = Texture{12,12};
    }
    PostInitialise();
}

// common initialisation handled here
void Canvas::Initialise(const std::wstring& saveFolder, const std::wstring& fileName) {
    this->fileName = fileName;
    filePath = saveFolder + this->fileName;
    textureSaved = true;
    currentBrushType = STARTING_TOOL;
    brushSize = START_BRUSH_SIZE;
    currentPixel = { STARTING_GLYPH, STARTING_COLOUR };
    drawPixel = { STARTING_GLYPH, STARTING_COLOUR };
    deletePixel = { L'X', FG_WHITE };
    cutPixel = { L'.', FG_DARK_YELLOW };
    bottomRight = { static_cast<short>(topLeft.X + TexturePainter::CANVAS_DISPLAY_WIDTH),
                    static_cast<short>(topLeft.Y + TexturePainter::CANVAS_DISPLAY_HEIGHT) };

    // all tool states
    toolStates[ToolType::BRUSH_BLOCK] = new BlockBrushState(*this);
    toolStates[ToolType::BRUSH_LINE] = new LineBrushState(*this);
    toolStates[ToolType::BRUSH_RECT] = new RectBrushState(*this);
    toolStates[ToolType::BRUSH_RECT_FILLED] = new FilledRectBrushState(*this);
    toolStates[ToolType::BRUSH_CIRCLE] = new CircleBrushState(*this);
    toolStates[ToolType::BRUSH_CIRCLE_FILLED] = new FilledCircleBrushState(*this);
    toolStates[ToolType::BRUSH_COPY] = new CopyBrushState(*this);
}

// post initialisation settings
void Canvas::PostInitialise()
{   
    // create stroke texture after background created or loaded
    currentBrushStrokeTexture = Texture{ backgroundTexture.GetWidth(), backgroundTexture.GetHeight() };
    // starting tool
    SwitchTool(STARTING_TOOL);

    PopulateColourButtonsContainer();
    PopulateToolButtonsContainer();
    UpdateActiveButtons();

}

// destructor
Canvas::~Canvas() {
    for (auto& toolState : toolStates) {
        delete toolState.second;
    }

    delete clipboardTextureSample;
    delete colourButtonsContainer;
    delete brushButtonsContainer;

}

// delete the static pointers
void Canvas::CleanUpStaticPointers()
{
    delete sharedClipboardTextureSample;
    delete deleteToolIcon;
    delete blockToolIcon;
    delete increaseToolIcon;
    delete decreaseToolIcon;
    delete lineToolIcon;
    delete rectToolIcon;
    delete rectFillToolIcon;
    delete circleToolIcon;
    delete circleFillToolIcon;
    delete clipboardToolIcon;
    delete clipboardToolToggleIcon;
    delete clipboardToolToggle2Icon;
    delete sharedClipboardSaveIcon;
    delete sharedClipboardDeleteIcon;
    delete sharedClipboardLoadIcon;
    delete sharedClipboardLoadedIcon;

    sharedClipboardTextureSample = nullptr;
    deleteToolIcon = nullptr;
    blockToolIcon = nullptr;
    increaseToolIcon = nullptr;
    decreaseToolIcon = nullptr;
    rectToolIcon = nullptr;
    rectFillToolIcon = nullptr;
    lineToolIcon = nullptr;
    clipboardToolIcon = nullptr;
    clipboardToolToggleIcon = nullptr;
    clipboardToolToggle2Icon = nullptr;
    sharedClipboardSaveIcon = nullptr;
    sharedClipboardDeleteIcon = nullptr;
    sharedClipboardLoadIcon = nullptr;
    sharedClipboardLoadedIcon = nullptr;
}

bool Canvas::SaveTexture(const std::wstring& filePath)
{
    if (backgroundTexture.SaveAs(filePath))
    {
        textureSaved = true;
        return true;
    }
    else
        return false;
}

bool Canvas::LoadTexture(const std::wstring& filePath)
{
    if (backgroundTexture.LoadFrom(filePath))
    {
        textureSaved = true;
        return true;
    }
    else
        return false;
}

const std::wstring& Canvas::GetFileName()
{
    return fileName;
}

const std::wstring& Canvas::GetFilePath()
{
    return filePath;
}

bool Canvas::GetSavedState()
{
    return textureSaved;
}

void Canvas::PopulateColourButtonsContainer()
{
    // container for colour buttons
    colourButtonsContainer = new ButtonContainer(drawingClass, COLOUR_BUTTON_XPOS, COLOUR_BUTTON_YPOS, 2, 9);

    // populate it 
    for (short colour = 0; colour < 8; ++colour) // For simplicity directly using the index as the color here and OR with FG_INTENSITY.
    {
        colourButtonsContainer->AddButton(true, 5, 5, colour, [this, colour]() { SetBrushColour(colour); });
        colourButtonsContainer->AddButton(true, 5, 5, colour | FG_INTENSITY, [this, colour]() { SetBrushColour(colour | FG_INTENSITY); });
    }

    colourButtonsContainer->AddButton(true, deleteToolIcon, [this]() { SetBrushToDelete(); });
}

void Canvas::PopulateToolButtonsContainer()
{
    // container for tool bottons
    brushButtonsContainer = new ButtonContainer(drawingClass, BRUSH_BUTTON_XPOS, BRUSH_BUTTON_YPOS, 12, 1);

    // populate tool button container
    brushButtonsContainer->AddButton(true, blockToolIcon, [this]() { SwitchTool(ToolType::BRUSH_BLOCK); });
    brushButtonsContainer->AddButton(false, increaseToolIcon, [this]() { ChangeBrushSize(1); });
    brushButtonsContainer->AddButton(false, decreaseToolIcon, [this]() { ChangeBrushSize(-1); });
    brushButtonsContainer->AddButton(true, lineToolIcon, [this]() { SwitchTool(ToolType::BRUSH_LINE); });
    brushButtonsContainer->AddButton(true, rectToolIcon, [this]() { SwitchTool(ToolType::BRUSH_RECT); });
    brushButtonsContainer->AddButton(true, rectFillToolIcon, [this]() { SwitchTool(ToolType::BRUSH_RECT_FILLED); });
    brushButtonsContainer->AddButton(true, circleToolIcon, [this]() { SwitchTool(ToolType::BRUSH_CIRCLE); });
    brushButtonsContainer->AddButton(true, circleFillToolIcon, [this]() { SwitchTool(ToolType::BRUSH_CIRCLE_FILLED); });
    brushButtonsContainer->AddButton(true, clipboardToolIcon, [this]() { SwitchTool(ToolType::BRUSH_COPY); });
    
    brushButtonsContainer->AddButton(false, false, clipboardToolToggle2Icon, clipboardToolToggleIcon, [this]() { ToggleClipboardOption(); });
    // add the activating tool
    brushButtonsContainer->AddNewToolToButtonActivateList(ToolType::BRUSH_COPY);
    // link button appearance to paintPartialSample bool
    brushButtonsContainer->AddExternalBoolPtr(&paintPartialSample);

    brushButtonsContainer->AddButton(false, false, sharedClipboardSaveIcon, sharedClipboardDeleteIcon, [this]() { ManageSharedClipboardTextureSample(); });
    // add the activating tool
    brushButtonsContainer->AddNewToolToButtonActivateList(ToolType::BRUSH_COPY);
    // link button appearance to sharedClipboardFilled bool
    brushButtonsContainer->AddExternalBoolPtr(&sharedClipboardFilled);


    brushButtonsContainer->AddButton(false, false, sharedClipboardLoadIcon, sharedClipboardLoadedIcon, [this]() { CopySharedClipboardToCurrentClipboard(); });
    // add the activating tool
    brushButtonsContainer->AddNewToolToButtonActivateList(ToolType::BRUSH_COPY);
    // link button appearance to sharedClipboardFilled bool
    brushButtonsContainer->AddExternalBoolPtr(&sharedClipboardFilled);
}

void Canvas::HandleAnyButtonsClicked(COORD mouseCoords)
{
    colourButtonsContainer->HandleMouseClick(mouseCoords);
    brushButtonsContainer->HandleMouseClick(mouseCoords);
}

// static method to populate all texture canvases icons 
void Canvas::InitialiseTextures() {
    deleteToolIcon = new Texture(L"./ToolIcons/delete_tool_icon.txr");
    blockToolIcon = new Texture(L"./ToolIcons/block_tool_icon.txr");
    increaseToolIcon = new Texture(L"./ToolIcons/increase_tool_icon.txr");
    decreaseToolIcon = new Texture(L"./ToolIcons/decrease_tool_icon.txr");
    lineToolIcon = new Texture(L"./ToolIcons/line_tool_icon.txr");
    rectToolIcon = new Texture(L"./ToolIcons/rect_tool_icon.txr");
    rectFillToolIcon = new Texture(L"./ToolIcons/rect_fill_tool_icon.txr");
    circleToolIcon = new Texture(L"./ToolIcons/circle_tool_icon.txr");
    circleFillToolIcon = new Texture(L"./ToolIcons/circle_fill_tool_icon.txr");
    clipboardToolIcon = new Texture(L"./ToolIcons/copy_tool_icon.txr");
    clipboardToolToggleIcon = new Texture(L"./ToolIcons/copy_tool_toggle_icon.txr");
    clipboardToolToggle2Icon = new Texture(L"./ToolIcons/copy_tool_toggle2_icon.txr");
    sharedClipboardSaveIcon = new Texture(L"./ToolIcons/shared_clipboard_save_icon.txr");
    sharedClipboardDeleteIcon = new Texture(L"./ToolIcons/shared_clipboard_delete_icon.txr");
    sharedClipboardLoadIcon = new Texture(L"./ToolIcons/shared_clipboard_load_icon.txr");
    sharedClipboardLoadedIcon = new Texture(L"./ToolIcons/shared_clipboard_loaded_icon.txr");
}

int Canvas::GetIllumination()
{
    return backgroundTexture.GetIllumination();
}

int Canvas::GetZoomLevel()
{
    return zoomLevel;
}

void Canvas::SetZoomLevel(int newZoomLevel)
{
    zoomLevel = newZoomLevel;
}

void Canvas::IncreaseZoomLevel()
{
    zoomLevel = (zoomLevel % 3) + 1;
}

void Canvas::DecreaseZoomLevel() {
    if (zoomLevel == 1) {
        zoomLevel = 3; // Wrap around to the max zoom level
    }
    else {
        zoomLevel -= 1; // Decrease zoom level by 1
    }
}

int Canvas::GetTextureWidth()
{
    return backgroundTexture.GetWidth();
}

int Canvas::GetTextureHeight()
{
    return backgroundTexture.GetHeight();
}

int Canvas::GetBrushSize()
{
    return brushSize;
}

short Canvas::GetBrushColour()
{
    return drawPixel.Attributes;
}

void Canvas::SetBrushColour(short newColour)
{   
    currentPixel.Attributes = newColour;
    // also reset glyph to solid_pixel
    currentPixel.Char.UnicodeChar = PIXEL_SOLID;

    drawPixel = currentPixel;
}

void Canvas::SetBrushColourAndGlyph(short colour, short glyph)
{
    currentPixel.Attributes = colour;
    currentPixel.Char.UnicodeChar = glyph;

    drawPixel = currentPixel;
}

void Canvas::SwitchTool(ToolType type) {
    // reset any tool specific logic
    if (currentToolState)
        currentToolState->ResetTool();
    // search for new tool
    auto it = toolStates.find(type);
    if (it != toolStates.end()) {
        currentToolState = it->second;  // Update the current tool state pointer
        UpdateActiveButtons();
    }
    else
        drawingClass.AddToLog(L"Unable to switch tools.");
}

bool Canvas::UpdateActiveButtons()
{   
    // update buttons to check if active
    if (brushButtonsContainer != nullptr)
    {
        brushButtonsContainer->UpdateButtonActive(GetCurrentToolType());
        return true;

    }
    else
    {
        drawingClass.AddToLog(L"Unable to update buttons in brushButtonContainer as nullptr.");
        return false;
    }
}

ToolType Canvas::GetCurrentToolType()
{
    for (const auto& pair : toolStates) {
        if (pair.second == currentToolState) {
            return pair.first; // Return the matching ToolType
        }
    }
    // return invalid if currentToolState doesn't return
    drawingClass.AddToLog(L"Unable to return a ToolType as currentToolState not recognised.");
    return ToolType::BRUSH_INVALID;
}

void Canvas::ToggleClipboardOption()
{
    // Check if the currentToolState is BRUSH_COPY 
    if (toolStates[ToolType::BRUSH_COPY] == currentToolState)
    {
        // activate initial click so brush ready to paint to brush texture
        toolStates.at(ToolType::BRUSH_COPY)->ToggleToolState();
        paintPartialSample = toolStates.at(ToolType::BRUSH_COPY)->GetToggleState();
    }
    else {
        // Log an error if the BRUSH_COPY tool is not found in toolStates
        drawingClass.AddToLog(L"Unable to toggle BRUSH_COPY tool as it is not current tool state.");
    }
}

void Canvas::SetBrushToDelete()
{
        drawPixel = deletePixel;
}

void Canvas::SetBrushToCurrentPixel()
{
    drawPixel = currentPixel;
}

int Canvas::ChangeCanvasOffset(COORD change)
{
    // Update the canvasOffset with the change
    canvasViewOffset.X += change.X;
    canvasViewOffset.Y += change.Y;

    int canvasWidth = backgroundTexture.GetWidth();
    int canvasHeight = backgroundTexture.GetHeight();

    // Clamp canvasOffset.X to be within 0 and canvasWidth
    if (canvasViewOffset.X < 0)
        canvasViewOffset.X = 0;
    else if (canvasViewOffset.X > canvasWidth  - 1)
        canvasViewOffset.X = canvasWidth - 1;

    // Clamp canvasOffset.Y to be within 0 and canvasHeight
    if (canvasViewOffset.Y < 0)
        canvasViewOffset.Y = 0;
    else if (canvasViewOffset.Y > canvasHeight - 1)
        canvasViewOffset.Y = canvasHeight - 1;

    // Return 0 for success  - might add feedback if hitting edge
    return 0;
}

bool Canvas::AreCoordsWithinCanvas(COORD coords)
{
    // Check if the mouse coordinates are within the zoom-adjusted canvas boundaries
    if (coords.X >= topLeft.X && coords.X < bottomRight.X && coords.Y >= topLeft.Y && coords.Y < bottomRight.Y)
        return true;
    else
        return false;
}

void Canvas::HandleLeftMouseClick(COORD mouseCoords) {
    COORD textureCoords = coordinateStrategy->ConvertScreenToTexture(mouseCoords);
    if (currentToolState) {
        currentToolState->HandleBrushStroke(textureCoords);
    }
}
void Canvas::HandleLeftMouseRelease(COORD mouseCoords) {
    COORD textureCoords = coordinateStrategy->ConvertScreenToTexture(mouseCoords);
    if (currentToolState) {
        currentToolState->HandleMouseRelease(textureCoords);
    }
}

// when left button lifted
void Canvas::SetBrushTextureToBackground()
{
    // create a new brushstroke to add to the brushManager
    BrushstrokeCommand* newStroke = new BrushstrokeCommand(*this, CaptureDifferential());
    brushMangager.PerformAction(newStroke);

    // clear to start new brushstroke
    ClearCurrentBrushstrokeTexture();
}

// used to make the (brushStroke) differential between the brushTexture and backgroundTexture
Canvas::Brushstroke Canvas::CaptureDifferential() {
    Canvas::Brushstroke stroke;

    int width = backgroundTexture.GetWidth();
    int height = backgroundTexture.GetHeight();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Capture the glyphs and colours from both textures
            short bgGlyph = backgroundTexture.GetGlyph(x, y);
            short cbstGlyph = currentBrushStrokeTexture.GetGlyph(x, y);
            short bgColour = backgroundTexture.GetColour(x, y);
            short cbstColour = currentBrushStrokeTexture.GetColour(x, y);

            // Check if there is a difference and the glyph in the brush stroke is not a space (otherwise whole texture is captured)
            if ((bgGlyph != cbstGlyph || bgColour != cbstColour) && cbstGlyph != L' ') {
                stroke.changes.push_back(TextureChangePixel{
                    x, y,
                    bgGlyph,
                    cbstGlyph,
                    bgColour,
                    cbstColour
                    });
            }
        }
    }
    return stroke;
}

// used to grab the background backgroundTexture within a rectangle
void Canvas::AddTextureSampleToClipboard(COORD topLeft, COORD bottomRight) {
    TextureSample* sample = new TextureSample;

    // Determine the actual top-left and bottom-right corners considering any orientation
    COORD actualTopLeft = { std::min(topLeft.X, bottomRight.X), std::min(topLeft.Y, bottomRight.Y) };
    COORD actualBottomRight = { std::max(topLeft.X, bottomRight.X), std::max(topLeft.Y, bottomRight.Y) };

    // Clamp the coordinates within the texture's dimensions
    actualBottomRight.X = std::min(actualBottomRight.X, static_cast<short>(backgroundTexture.GetWidth() - 1));
    actualBottomRight.Y = std::min(actualBottomRight.Y, static_cast<short>(backgroundTexture.GetHeight() - 1));

    // Set sample width and height
    sample->width = actualBottomRight.X - actualTopLeft.X + 1;
    sample->height = actualBottomRight.Y - actualTopLeft.Y + 1;

    for (int y = actualTopLeft.Y; y <= actualBottomRight.Y; ++y) {
        for (int x = actualTopLeft.X; x <= actualBottomRight.X; ++x) {
            // Get the glyph and colour from the background texture
            short glyph = backgroundTexture.GetGlyph(x, y);
            short colour = backgroundTexture.GetColour(x, y);
            
            // if an empty pixel then make it a 'delete' pixel
            if (glyph == L' ')
            {
                glyph = deletePixel.Char.UnicodeChar;
                colour = deletePixel.Attributes;
            }

            // Record the texture's glyph and colour, normalised to start from (0, 0)
            sample->pixels.push_back(PixelSample{
                x - actualTopLeft.X, // Normalise positions
                y - actualTopLeft.Y,
                glyph,
                colour
                });
        }
    }

    // Replace the old clipboard texture with the new one
    delete clipboardTextureSample; // Safely delete the old texture
    clipboardTextureSample = sample; // Point to the new texture
}

// Method to get the current texture sample from the active tool state
Canvas::TextureSample* Canvas::GetClipboardTextureSample() const {
    return clipboardTextureSample;
}

// Method to set a new texture sample into the clipboard
void Canvas::SetClipboardTextureSample(TextureSample* newTextureSample) {
    if (newTextureSample != clipboardTextureSample) {
        delete clipboardTextureSample; // Safely delete the old texture
        // Deep copy the new texture with default copy construtor (don't want to point to same data)
        clipboardTextureSample = new TextureSample(*newTextureSample);
    }
}

bool Canvas::GetSharedClipboardTextureState()
{
    return sharedClipboardTextureSample == nullptr ? false : true;
}

// this method will either
// - copy sample to shared clipboard if it is empty 
// - OR delete shared clipboard if already populated
void Canvas::ManageSharedClipboardTextureSample()
{
    // Check if the currentToolState is BRUSH_COPY 
    if (toolStates[ToolType::BRUSH_COPY] == currentToolState)
    {
        // if shared clipboard is empty then populate with a deep copy of canvas clipboard (only if canvas clipb. not null)
        if (sharedClipboardTextureSample == nullptr && clipboardTextureSample != nullptr)
        {
            sharedClipboardTextureSample = clipboardTextureSample->Clone();  // Deep copy
        }
        // otherwise delete shared clipboard and set to nullptr
        else {
            delete sharedClipboardTextureSample;  // Clean up existing shared clipboard sample
            sharedClipboardTextureSample = nullptr;  // No current texture to copy
        }
    }
    else {
        drawingClass.AddToLog(L"Unable to add canvas texture sample to shared clipboard as BRUSH_COPY not active tool OR nullptr sample.");
    }
}

// 
void Canvas::CopySharedClipboardToCurrentClipboard()
{
    // Check if the currentToolState corresponds the copy brush, and insn't a nullptr
    if (toolStates[ToolType::BRUSH_COPY] == currentToolState && sharedClipboardTextureSample != nullptr)
    {
        // activate initial click so brush ready to paint to brush texture
        toolStates.at(ToolType::BRUSH_COPY)->SetClicks(true);
        delete clipboardTextureSample;  // Clean up existing texture sample
        if (sharedClipboardTextureSample) {
            clipboardTextureSample = sharedClipboardTextureSample->Clone();  // Deep copy
        }
        else {
            clipboardTextureSample = nullptr;  // No shared texture to copy
        }
    }
    else {
        drawingClass.AddToLog(L"Unable to add shared texture sample to canvas clipboard as BRUSH_COPY not active tool OR nullptr sample.");
    }
}

void Canvas::ClearCurrentBrushstrokeTexture()
{
    currentBrushStrokeTexture.Clear();
}

// used by UndoRedoManager to apply the brushTexture to the background
void Canvas::ApplyBrushstrokeTextureToBackground(const Brushstroke& stroke) 
{
    textureSaved = false; // changed background texture

    for (const auto& change : stroke.changes) {

        short glyph = change.newGlyph;
        short colour = change.newColour;

        // check if brushstroke glyph signalling a delete with deletePixel glyph
        if (change.newGlyph == deletePixel.Char.UnicodeChar)
        {
            glyph = L' ';
            colour = FG_BLACK;
        }
            // Set the glyph and color at the specified position to their new values
        backgroundTexture.SetGlyph(change.x, change.y, glyph);
        backgroundTexture.SetColour(change.x, change.y, colour);
    }
}

// used by UndoRedoManager to apply the undo of the brushTexture to the background
void Canvas::ApplyUndoBrushstroke(const Brushstroke& stroke)
{
    textureSaved = false; // changed background texture

    for (const auto& change : stroke.changes) {
        // Set the glyph and color at the specified position to their new values
        backgroundTexture.SetGlyph(change.x, change.y, change.oldGlyph);
        backgroundTexture.SetColour(change.x, change.y, change.oldColour);
    }
}

void Canvas::ChangeBrushSize(int sizeChange)
{
    int newSize = (brushSize - 1 + sizeChange + MAX_BRUSH_SIZE) % MAX_BRUSH_SIZE + 1;
    brushSize = newSize;
}

void Canvas::PaintPoint(int x, int y)
{
    currentBrushStrokeTexture.SetPixel(x, y, drawPixel);
}

// paint a glyph over existing background colour
void Canvas::PaintGlyph(int x, int y)
{
    // turn background texture colour (will be a fg_colour) into a background colour by bitshifting)
    short bgColour = backgroundTexture.GetColour(x, y) << 4;

    currentBrushStrokeTexture.SetGlyph(x, y, cutPixel.Char.UnicodeChar);
    currentBrushStrokeTexture.SetColour(x, y, cutPixel.Attributes | bgColour);
}

void Canvas::PaintLine(int x0, int y0, int x1, int y1, int lineThickness)
{ // Bresenham's line algorithm
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; // error value e_xy

    while (true) {
        PaintBlock(x0, y0, brushSize); // Use DisplayBlockOnCanvas to draw a block at each point

        if (x0 == x1 && y0 == y1) break; // Check if the end point is reached
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Canvas::PaintBlock(int x, int y, int sideLength)
{
    for (size_t i{ 0 }; i < sideLength; i++)
        for (size_t j{ 0 }; j < sideLength; j++)
            PaintPoint(x + i, y + j);

}

void Canvas::PaintBlockCentred(int x, int y, int sideLength)
{
    // Draws a square centered at (x, y) with the specified size
    int half = sideLength / 2;
    for (int dx = -half; dx <= half; ++dx) {
        for (int dy = -half; dy <= half; ++dy) {
            PaintPoint(x + dx, y + dy);
        }
    }
}

void Canvas::PaintRectangleCoords(int x0, int y0, int x1, int y1, bool filled, int lineWidth)
{
    // Normalize coordinates
    int left = std::min(x0, x1);
    int top = std::min(y0, y1);
    int right = std::max(x0, x1);
    int bottom = std::max(y0, y1);

    if (filled) {
        for (int y = top; y <= bottom; ++y) {
            for (int x = left; x <= right; ++x) {
                PaintPoint(x, y);
            }
        }
    }
    else {
        // Top and bottom sides
        for (int i = 0; i < lineWidth; ++i) {
            for (int x = left; x <= right; ++x) {
                PaintPoint(x, top + i); // Top side
                PaintPoint(x, bottom - i); // Bottom side
            }
        }
        // Left and right sides
        for (int i = 0; i < lineWidth; ++i) {
            for (int y = top; y <= bottom; ++y) {
                PaintPoint(left + i, y); // Left side
                PaintPoint(right - i, y); // Right side
            }
        }
    }
}

// paints glyphs over the texture
void Canvas::PaintRectangleGlyphs(int x0, int y0, int x1, int y1, bool filled, int lineWidth)
{
    // Normalize coordinates
    int left = std::min(x0, x1);
    int top = std::min(y0, y1);
    int right = std::max(x0, x1);
    int bottom = std::max(y0, y1);

    if (filled) {
        for (int y = top; y <= bottom; ++y) {
            for (int x = left; x <= right; ++x) {
                PaintGlyph(x, y); // Use PaintGlyph to paint the filled rectangle
            }
        }
    }
    else {
        // Top and bottom sides
        for (int i = 0; i < lineWidth; ++i) {
            for (int x = left; x <= right; ++x) {
                PaintGlyph(x, top + i); // Top side
                PaintGlyph(x, bottom - i); // Bottom side
            }
        }
        // Left and right sides
        for (int i = 0; i < lineWidth; ++i) {
            for (int y = top; y <= bottom; ++y) {
                PaintGlyph(left + i, y); // Left side
                PaintGlyph(right - i, y); // Right side
            }
        }
    }
}

// ChatGPT used (with guidance!) to create this method
void Canvas::PaintCircleCoords(int centerX, int centerY, int pointX, int pointY, bool filled, int pointSize) {
    // Calculate the radius using the distance formula
    int radius = static_cast<int>(std::round(std::sqrt((pointX - centerX) * (pointX - centerX) + (pointY - centerY) * (pointY - centerY))));

    // Draw circle outline using Midpoint Circle Algorithm with large points
    int x = radius, y = 0;
    int decisionOver2 = 1 - x;

    while (y <= x) {
        PaintBlockCentred(centerX + x, centerY + y, pointSize ); // Octant 1
        PaintBlockCentred(centerX + y, centerY + x, pointSize ); // Octant 2
        PaintBlockCentred(centerX - y, centerY + x, pointSize ); // Octant 3
        PaintBlockCentred(centerX - x, centerY + y, pointSize ); // Octant 4
        PaintBlockCentred(centerX - x, centerY - y, pointSize ); // Octant 5
        PaintBlockCentred(centerX - y, centerY - x, pointSize ); // Octant 6
        PaintBlockCentred(centerX + y, centerY - x, pointSize ); // Octant 7
        PaintBlockCentred(centerX + x, centerY - y, pointSize ); // Octant 8
        ++y;
        if (decisionOver2 <= 0) {
            decisionOver2 += 2 * y + 1;   // Change for y -> y+1
        }
        else {
            --x;
            decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
        }
    }
    // if filled
    if (filled) {
        // Draw a filled circle by filling horizontal lines across each vertical position
        for (int y = -radius; y <= radius; ++y) {
            int lineWidth = static_cast<int>(std::sqrt(radius * radius - y * y));
            for (int x = -lineWidth; x <= lineWidth; ++x) {
                PaintBlockCentred(centerX + x, centerY + y, pointSize);
            }
        }
    }

}

void Canvas::PaintClipboardTextureSample(COORD topLeft, bool partialSample)
{
    if (clipboardTextureSample) // if not nullptr
    {
        for (const auto& pixel : clipboardTextureSample->pixels) {
            // Calculate the new position by adding the top-left offset
            int newX = pixel.x + topLeft.X;
            int newY = pixel.y + topLeft.Y;

            short glyph = pixel.glyph;
            short colour = pixel.colour;

            // Ensure the new positions are within the texture's dimensions before applying the change
            if (newX < currentBrushStrokeTexture.GetWidth() && newY < currentBrushStrokeTexture.GetHeight()) {
                // If partialSample is false, draw all pixels
                if (!partialSample) {
                    currentBrushStrokeTexture.SetGlyph(newX, newY, glyph);
                    currentBrushStrokeTexture.SetColour(newX, newY, colour);
                }
                // If partialSample is true, only draw pixels that are not the deletePixel
                else if (glyph != deletePixel.Char.UnicodeChar) {
                    currentBrushStrokeTexture.SetGlyph(newX, newY, glyph);
                    currentBrushStrokeTexture.SetColour(newX, newY, colour);
                }
            }
        }
    }

}

void Canvas::DrawCanvas()
{
    drawingClass.WriteStringToBuffer(topLeft.X, topLeft.Y - 3, L"ZOOM: " + std::to_wstring(zoomLevel) + L'. ');
    drawingClass.WriteStringToBuffer(topLeft.X + 8, topLeft.Y - 3, L"X: -   Y: -");

    COORD mouseCoords = drawingClass.GetMousePosition();

    // if coords withing canvas diplay (index starting from 1 for user)
    if (AreCoordsWithinCanvas(mouseCoords))
    {
        COORD textureCoords{ coordinateStrategy->ConvertScreenToTexture(mouseCoords) };
        drawingClass.WriteStringToBuffer(topLeft.X + 11, topLeft.Y - 3, std::to_wstring(textureCoords.X + 1));
        drawingClass.WriteStringToBuffer(topLeft.X + 18, topLeft.Y - 3, std::to_wstring(textureCoords.Y + 1));
    }

    // currently drawing offset for debugging
    drawingClass.WriteStringToBuffer(topLeft.X, topLeft.Y - 2, std::to_wstring(canvasViewOffset.X) + L' ' + std::to_wstring(canvasViewOffset.Y));

    // draw non texture background
    drawingClass.DrawRectangleEdgeLength(topLeft.X, topLeft.Y, TexturePainter::CANVAS_DISPLAY_WIDTH, TexturePainter::CANVAS_DISPLAY_HEIGHT, FG_BLUE, true, L'.');

    // add texture to screen buffer
    drawingClass.DrawSectionOfTextureToScreen(backgroundTexture, 
                                                topLeft,            // where to draw on screen
                                                canvasViewOffset,   // top left of texture to start
                                                coordinateStrategy->ConvertScreenToTexture(bottomRight), // bottom right of texture to stop
                                                zoomLevel,
                                                true);

    // draw brush stroke as it is made
    drawingClass.DrawPartialTextureToScreen(currentBrushStrokeTexture, 
                                                coordinateStrategy->AdjustForOffset(topLeft),
                                                zoomLevel);

    // draw border around canvas display panel
    drawingClass.DrawRectangleEdgeLength(topLeft.X - 1, topLeft.Y - 1, TexturePainter::CANVAS_DISPLAY_WIDTH + 2, TexturePainter::CANVAS_DISPLAY_HEIGHT + 2, FG_RED, false, PIXEL_HALF);
    
    DrawButtons();

    // draw mouse pointer
    if (AreCoordsWithinCanvas(mouseCoords))
        currentToolState->DisplayPointer(mouseCoords);

}

// update called in parent class - updates external boolean pointers and button appearences
void Canvas::UpdateButtons()
{
    // Updating if button active occurs in SwitchTool()
    // update bools used as external pointers in buttons
    sharedClipboardFilled = GetSharedClipboardTextureState();
    //update appearance
    brushButtonsContainer->UpdateButtonAppearance();
}

void Canvas::DrawButtons()
{
    colourButtonsContainer->DrawButtons(drawingClass.GetMousePosition());
    brushButtonsContainer->DrawButtons(drawingClass.GetMousePosition());
}

void Canvas::DisplayBrushPointer(COORD mouseCoords, bool justOnePixel)
{
    // converts the mouse coordinates to the texture and back to screen to 'snap' pointer to correct pixels - accounts for zoom/scrolling etc
    COORD pointerCoords{ coordinateStrategy->ConvertCoordsToCanvasPixel(mouseCoords) };

    int size{ brushSize };

    if (justOnePixel)
        size = 1;

    drawingClass.DrawBlock(pointerCoords.X, pointerCoords.Y, size * zoomLevel, drawPixel.Attributes, PIXEL_HALF);
}

void Canvas::UndoLastCommand()
{
    brushMangager.Undo();
}

void Canvas::RedoLastCommand()
{
    brushMangager.Redo();
}

int Canvas::GetSizeUndoStack()
{
    return brushMangager.GetUndoStackSize();
}

int Canvas::GetSizeRedoStack()
{
    return brushMangager.GetRedoStackSize();

}



