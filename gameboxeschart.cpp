#include "gameboxeschart.h"
#include "ui_gameboxeschart.h"

static constexpr auto box_iter_space = 60;

GameBoxesChart::GameBoxesChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameBoxesChart)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 300, 100);
    ui->gfxView->setScene(scene);
    GameResultBox* g1 = new GameResultBox();
    GameResultBox* g2 = new GameResultBox();
    GameResultBox* g3 = new GameResultBox();
    GameResultBox* g4 = new GameResultBox();
    GameResultBox* g5 = new GameResultBox();
    boxes << g1 << g2 << g3 << g4 << g5;

    QPen connectorPen{Qt::blue};
    connectorPen.setWidth(3);
    int i = 0;
    for(auto& box : boxes) {
        box->setPos(i*box_iter_space, 25);
        scene->addItem(box);
        ++i;
    }

    for(auto i = 0; i < boxes.size(); ++i) {
        if(i <= boxes.size()-2)
            scene->addLine(connecting_line(*boxes[i], *boxes[i+1]), connectorPen);
    }
    teamNameItem = nullptr;
}

GameBoxesChart::~GameBoxesChart()
{
    delete ui;
    for(auto& box : boxes) delete box;
}

void GameBoxesChart::update_data(const TeamStats &ts)
{
    std::cout << "Updating game boxes chart data" << std::endl;
    int index = 0;
    team = ts.team_name().c_str();
    for(auto [begin, end] = ts.games_range_from_back(5); begin != end; begin++) {
        boxes[index]->set_gameid(begin->game_id());
        if(begin->winning_team() == ts.team_name()) {
            boxes[index]->set_won();
        } else {
            boxes[index]->set_won(false);
        }
        index++;
    }
    if(teamNameItem != nullptr) scene->removeItem(teamNameItem);
    teamNameItem = scene->addText(team);
}

void GameBoxesChart::register_connections(MainWindow &window)
{
    for(auto& box : boxes) {
        connect(box->get_handle(), &SignalsHandler::gameDataPopUp, &window, &MainWindow::game_data_popup);
    }
}
