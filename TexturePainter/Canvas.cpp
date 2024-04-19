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



// Primary constructor that does all the work
Canvas::Canvas(TexturePainter& drawer, int width, int height, int illumination, const std::wstring& saveFolder, const std::wstring& fileName, short xPos, short yPos)
    : drawingClass{ drawer },
    backgroundTexture{ width, height, illumination },
    topLeft{ xPos, yPos },
    canvasViewOffset{ 0, 0 },
    zoomLevel{ START_ZOOM_LEVEL },
    currentToolState{ nullptr },
    clipboardTexture{ new TextureSample },
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
        backgroundTexture = Texture{};
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
    deletePixel = { L'X', FG_WHITE };
    cutPixel = { L'.', FG_DARK_YELLOW };
    bottomRight = { static_cast<short>(topLeft.X + TexturePainter::CANVAS_DISPLAY_WIDTH),
                    static_cast<short>(topLeft.Y + TexturePainter::CANVAS_DISPLAY_HEIGHT) };

    toolStates[ToolType::BRUSH_BLOCK] = new BlockBrushState(*this);
    toolStates[ToolType::BRUSH_LINE] = new LineBrushState(*this);
    toolStates[ToolType::BRUSH_RECT] = new RectBrushState(*this);
    toolStates[ToolType::BRUSH_RECT_FILLED] = new FilledRectBrushState(*this);
    toolStates[ToolType::BRUSH_COPY] = new CopyBrushState(*this);

}

// post initialisation settings
void Canvas::PostInitialise()
{   
    // create stroke texture after background created or loaded
    currentBrushStrokeTexture = Texture{ backgroundTexture.GetWidth(), backgroundTexture.GetHeight() };
    // starting tool
    SwitchTool(STARTING_TOOL);
}

// destructor
Canvas::~Canvas() {
    for (auto& toolState : toolStates) {
        delete toolState.second;
    }

    delete clipboardTexture;
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
    return currentPixel.Attributes;
}

void Canvas::SetBrushColour(short newColour)
{   
    currentPixel.Attributes = newColour;

    // also reset glyph to solid_pixel
    currentPixel.Char.UnicodeChar = PIXEL_SOLID;
}

void Canvas::SetBrushColourAndGlyph(short colour, short glyph)
{
    currentPixel.Attributes = colour;
    currentPixel.Char.UnicodeChar = glyph;
}

void Canvas::SwitchTool(ToolType type) {
    // reset any tool specific logic
    if (currentToolState)
        currentToolState->ResetTool();
    // serach for new tool
    auto it = toolStates.find(type);
    if (it != toolStates.end()) {
        currentToolState = it->second;  // Update the current tool state pointer
    }
    else
    {
        drawingClass.AddToLog(L"Unable to switch tools.");
    }
}

void Canvas::ToggleCurrentToolOption()
{
    currentToolState->ToggleOption();
}

void Canvas::SetBrushToDelete()
{
        currentPixel = deletePixel;
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
    delete clipboardTexture; // Safely delete the old texture
    clipboardTexture = sample; // Point to the new texture
}

// Method to get the current texture sample from the active tool state
Canvas::TextureSample* Canvas::GetClipboardTextureSample() const {
    return clipboardTexture;
}

// Method to set a new texture sample into the clipboard
void Canvas::SetClipboardTextureSample(TextureSample* newTextureSample) {
    if (newTextureSample != clipboardTexture) {
        delete clipboardTexture; // Safely delete the old texture
        // Deep copy the new texture with default copy construtor (don't want to point to same data)
        clipboardTexture = new TextureSample(*newTextureSample);
    }
}

void Canvas::ClearCurrentBrushstrokeTexture() {
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
    int newSize = (brushSize - 1 + sizeChange + 5) % 5 + 1;
    brushSize = newSize;
}

void Canvas::PaintPoint(int x, int y)
{
    currentBrushStrokeTexture.SetPixel(x, y, currentPixel);
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

void Canvas::PaintRectangleCoords(int x0, int y0, int x1, int y1, bool filled, int lineWidth) {
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
void Canvas::PaintRectangleGlyphs(int x0, int y0, int x1, int y1, bool filled, int lineWidth) {
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

void Canvas::PaintClipboardTextureSample(COORD topLeft, bool partialSample)
{
    for (const auto& pixel : clipboardTexture->pixels) {
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
    
    // draw mouse pointer
    if (AreCoordsWithinCanvas(mouseCoords))
        currentToolState->DisplayPointer(mouseCoords);

}

void Canvas::DisplayBrushPointer(COORD mouseCoords)
{
    // converts the mouse coordinates to the texture and back to screen to 'snap' pointer to correct pixels - accounts for zoom/scrolling etc
    COORD pointerCoords{ coordinateStrategy->ConvertCoordsToCanvasPixel(mouseCoords) };
    drawingClass.DrawBlock(pointerCoords.X, pointerCoords.Y, brushSize * zoomLevel, currentPixel.Attributes, PIXEL_HALF);
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



