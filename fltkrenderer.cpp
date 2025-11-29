#include "fltkrenderer.h"

#include <cmath>

#include <sstream>
#include "vehicle.h"

FLTKRenderer::FLTKRenderer(World* world, int W, int H) : world(world)
{
    window = new Fl_Window(W, H);
    drawer = new FLTKCustomDrawer(world, W, H);
    window->end();
    window->show();
}

FLTKRenderer::~FLTKRenderer()
{
    drawer->redraw();
    while (Fl::wait())
        ;
    delete drawer;
    delete window;
}

void FLTKRenderer::render()
{
    clearScreen();
    drawer->redraw();
    Fl::check();
}

void FLTKRenderer::clearScreen()
{
    drawer->clearScreen();
}

FLTKCustomDrawer::FLTKCustomDrawer(World* world, int W, int H)
    : Fl_Box(0, 0, W, H, nullptr), world(world)
{
}

std::array<unsigned int, 6> displayColors = {FL_BLUE,   FL_CYAN, FL_GREEN,
                                             FL_YELLOW, FL_RED,  FL_MAGENTA};

void FLTKCustomDrawer::clearScreen()
{
    fl_color(FL_WHITE);
    fl_rectf(x(), y(), w(), h());
}

void FLTKCustomDrawer::drawVehicle(Vehicle const& vehicle)
{
    // Draw vehicles as blue triangles
    fl_color((displayColors[std::max(
        0ul, std::min(static_cast<size_t>(vehicle.getHealth() * 5 / 20.0),
                      displayColors.size() - 1))]));
    Vec2D      pos     = vehicle.getPosition();
    double     heading = vehicle.getVelocity().heading();
    DNA const& dna     = vehicle.getDNA();
    int        size    = 6;

    // Calculate triangle vertices
    int x1 = static_cast<int>(pos.x + cos(heading) * size);
    int y1 = static_cast<int>(pos.y + sin(heading) * size);
    int x2 = static_cast<int>(pos.x + cos(heading + 2.5) * size);
    int y2 = static_cast<int>(pos.y + sin(heading + 2.5) * size);
    int x3 = static_cast<int>(pos.x + cos(heading - 2.5) * size);
    int y3 = static_cast<int>(pos.y + sin(heading - 2.5) * size);

    // Draw the triangle
    fl_begin_polygon();
    fl_vertex(x1, y1);
    fl_vertex(x2, y2);
    fl_vertex(x3, y3);
    fl_end_polygon();

    // Draw an empty circle with a thin line to represent perception radius
    fl_color(FL_GREEN);
    fl_line_style(FL_SOLID, 2);
    fl_arc(pos.x - dna.perceptionRadius / 2, pos.y - dna.perceptionRadius / 2,
           dna.perceptionRadius, dna.perceptionRadius, 0, 360);
}

void FLTKCustomDrawer::drawFood(Food const& position)
{
    // Draw food as green circles
    fl_color(FL_GREEN);
    fl_pie(static_cast<int>(position.position.x) - 2,
           static_cast<int>(position.position.y) - 2, 4, 4, 0, 360);
}

void FLTKCustomDrawer::drawLivingWorld()
{
    for (auto& food : world->food) {
        drawFood(food);
    }

    for (auto& vehicle : world->vehicles) {
        drawVehicle(vehicle);
    }
}

void FLTKCustomDrawer::drawDeadWorld()
{
    fl_color(FL_RED);
    std::string message = "All vehicles have perished.";
    fl_font(FL_HELVETICA_BOLD, 24);
    int textWidth = fl_width(message.c_str()) + 1;
    fl_draw(message.c_str(), (w() - textWidth) / 2, h() / 2);
}

void FLTKCustomDrawer::draw()
{
    clearScreen();
    std::stringstream ss;
    ss << "(World: [" << world->width << "x" << world->height
       << "] seed: " << world->seed << ") "
       << "Vehicles: " << world->vehicles.size()
       << " | Food: " << world->food.size()
       << " | Dead Vehicles: " << world->deadCounter
       << " | Tick: " << world->tickCounter;
    fl_font(FL_HELVETICA_BOLD, 14);
    fl_color(FL_BLACK);
    fl_draw(ss.str().c_str(), 0, 0, w(), h(), FL_ALIGN_TOP_LEFT);

    if (world->vehicles.size() > 0) {
        drawLivingWorld();
    } else {
        drawDeadWorld();
    }
}
