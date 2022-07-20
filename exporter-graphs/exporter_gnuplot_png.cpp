#include "exporter_gnuplot_png.h"

#include <iostream>
#include <filesystem>

#include <fmt/core.h>

#include "gnuplot-iostream.h"

// use: bar, lines, points

//---------------------------------------------------------------------------------
TRedGnuplotPngExporter::TRedGnuplotPngExporter() {
}

TRedGnuplotPngExporter::~TRedGnuplotPngExporter() {
}

TRedExporter::Buffer TRedGnuplotPngExporter::exportToBuffer( TRedOutput* output, bool raw ) {

    auto params = output->options().save_params;

    int width = 800;
    int height = 600;
    std::string type = "boxes";

    if(params.find("width")!=params.end() ) {
        width = std::stoi(params.at("width"));
    }

    if(params.find("height")!=params.end() ) {
        height = std::stoi(params.at("height"));
    }

    if(params.find("use")!=params.end() ) {
        type = params.at("use");
    }

    if(type=="bar") {
        type = "boxes";
    }

    std::string file_name = output->options().save.value();

    auto dataset = output->dataset();

    Gnuplot gp;

    //std::string tmp_name = std::tmpnam(nullptr) + std::string(".png");

    if(output->dimension()==1 ) {
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

        auto data = boost::make_tuple(plot_labels,plot_values);

        std::string data_file = gp.file1d(data);

        std::stringstream command;

        command << fmt::format("set terminal png size {},{}\n", width, height);
        command << fmt::format("seset output '{}'\n",file_name);
        command << "set style data histograms\n";
        command << "set style histogram clustered\n";
        command << "set boxwidth 0.9 relative\n";
        command << "set style fill solid 1.0 border -1\n";
        command << fmt::format("set yrange [:{}]\n",max);
        command << fmt::format("set ylabel 'Count of {}'\n", var->fullName() );
        command << fmt::format("set title '{}'\n", var->label().empty()?var->fullName():var->label() );
        //command << "set ytics 10 nomirror\n";
        //command << fmt::format("set ytics 10\n");
        command << "set grid noxtics ytics\n";
        command << fmt::format("plot {} using 2:xtic(1) with {} title '{}'\n", data_file, type, var->fullName() );

        //std::cout<<command.str()<<std::endl;
        //std::cout.flush();

        gp << command.str();
        gp.flush();
    }

    return {};

//    // open file
//    std::string content = red::readWholeBinFile(file_name);
//
//    Buffer ret;
//
//    std::copy( content.begin(), content.end(), std::back_inserter(ret));
//
//    return ret;
}

