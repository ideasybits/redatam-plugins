//
// Created by jsalvador on 16/7/2022.
//

#include "exporter_jpg.h"

#include <fmt/core.h>
#include <chartdir.h>

#include <redatam/utils.h>

// use: bar, pie

//---------------------------------------------------------------------------------
TRedJpgExporter::TRedJpgExporter() {
    output = nullptr;

    this->width  = 800;
    this->height = 600;
}

TRedJpgExporter::~TRedJpgExporter() {
}

void TRedJpgExporter::bar1D() {

    auto dataset = output->dataset();

    auto var = output->variables().at(0);

    auto values = dataset->distinctValues(0);

    std::vector<char*> plot_labels;
    std::vector<double> plot_values;

    double max = 0;

    for (auto it: values) {

        auto label = var->findValueLabel(it);
        auto value = output->find({it});

        if( label.empty() ) {
            label = fmt::format("{}",value.value() );
        }

        if(value.value()>max) {
            max = value.value();
        }

        plot_labels.push_back( strdup(label.c_str()) );
        plot_values.push_back(value.value());
    }

    std::shared_ptr<XYChart> chart = std::make_shared<XYChart>(width,height);

    std::string title = var->label().empty()?var->fullName():var->label();
    chart->addTitle( title.c_str(), "Arial Bold", 16);//, 0x555555);
    chart->setPlotArea(100, 60, width-100, height-100, Chart::Transparent, -1, Chart::Transparent, 0xcccccc);

    // Set the x and y axis stems to transparent and the label font to 12pt Arial
    //chart->xAxis()->setColors(Chart::Transparent);
    //chart->yAxis()->setColors(Chart::Transparent);
    chart->xAxis()->setLabelStyle("Arial", 12);
    chart->yAxis()->setLabelStyle("Arial", 12);

    // Add a blue (0x6699bb) bar chart layer with transparent border using the given data
    auto data = DoubleArray(plot_values.data(), plot_values.size());
    BarLayer* layer = chart->addBarLayer(data, 0x6699bb);

    layer->setBorderColor(Chart::Transparent);
    // Set rounded corners for bars
    layer->setRoundedCorners();
    // Display labela on top of bars using 12pt Arial font
    layer->setAggregateLabelStyle("Arial", 10);

    // Set the labels on the x axis.
    auto lbl = StringArray(plot_labels.data(), plot_labels.size());
    chart->xAxis()->setLabels(lbl);

    // For the automatic y-axis labels, set the minimum spacing to 40 pixels.
    chart->yAxis()->setTickDensity(40);

    // Add a title to the y axis using dark grey (0x555555) 14pt Arial Bold font
    std::string ytitle = fmt::format("Count of {}", var->fullName() );
    chart->yAxis()->setTitle(ytitle.c_str(), "Arial", 12 );//, 0x555555);

    // Output the chart
    chart->makeChart(file_name.c_str());
}

void TRedJpgExporter::pie1D() {
    auto dataset = output->dataset();

    auto var = output->variables().at(0);

    auto values = dataset->distinctValues(0);

    std::vector<char*> plot_labels;
    std::vector<double> plot_values;

    double max = 0;

    for (auto it: values) {

        auto label = var->findValueLabel(it);
        auto value = output->find({it});

        if( label.empty() ) {
            label = fmt::format("{}",value.value() );
        }

        if(value.value()>max) {
            max = value.value();
        }

        plot_labels.push_back( strdup(label.c_str()) );
        plot_values.push_back(value.value());
    }

    std::shared_ptr<PieChart> chart = std::make_shared<PieChart>(width, height);

    // Set the center of the pie at (150, 100) and the radius to 80 pixels
    chart->setPieSize(width/2, height/2, (width>height?height-100:width-100)/2);

    chart->setRoundedFrame();
    chart->setDropShadow();
    chart->set3D(25); //3D
    chart->setLabelLayout(Chart::SideLayout, 16);
    chart->setLabelFormat("{={sector}+1}");

    std::string title = var->label().empty()?var->fullName():var->label();
    chart->addTitle( title.c_str(), "Arial Bold", 16);//, 0x555555);

    chart->setLabelStyle("Arial Bold", 10)->setBackground(Chart::Transparent, 0x444444);

    // add a legend box where the top left corner is at (330, 50)
    LegendBox* b = chart->addLegend(width-150, 60);
    b->setAlignment(Chart::Left);

    // modify the sector label format to show percentages only
    chart->setLabelFormat("{percent}%");

    // Set the pie data and the pie labels
    auto data = DoubleArray(plot_values.data(), plot_values.size());
    auto lbl = StringArray(plot_labels.data(), plot_labels.size());

    chart->setData(data, lbl);

    // Use rounded edge shading, with a 1 pixel white (FFFFFF) border
    chart->setSectorStyle(Chart::RoundedEdgeShading, 0xffffff, 1);

    // Output the chart
    chart->makeChart(file_name.c_str());
}

TRedExporter::Buffer TRedJpgExporter::exportToBuffer( TRedOutput* output, bool raw ) {
    this->output = output;

    auto params = output->options().save_params;

    std::string type = "bar";

    if(params.find("width")!=params.end() ) {
        width = std::stoi(params.at("width"));
    }

    if(params.find("height")!=params.end() ) {
        height = std::stoi(params.at("height"));
    }

    if(params.find("use")!=params.end() ) {
        type = params.at("use");
    }

    //file_name = output->options().save.value();
    file_name = std::tmpnam(nullptr) + std::string(".jpg");

    if(output->dimension()==1 ) {
        if( type=="bar" ) {
            bar1D();
        }
        if( type=="pie" ) {
            pie1D();
        }
    }

    // open file
    std::string content = red::readWholeBinFile(file_name);

    Buffer ret;

    std::copy( content.begin(), content.end(), std::back_inserter(ret));

    return ret;
}
