// Copyright (c) 2015-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bench/bench.h>

#include <args.h>
#include <crypto/sha256.h>
#include <key.h>
#include <validation.h>
#include <util.h>
#include <random.h>

#include <boost/lexical_cast.hpp>

#include <memory>

static const int64_t DEFAULT_BENCH_EVALUATIONS = 5;
static const char* DEFAULT_BENCH_FILTER = ".*";
static const char* DEFAULT_BENCH_SCALING = "1.0";
static const char* DEFAULT_BENCH_PRINTER = "console";
static const char* DEFAULT_PLOT_PLOTLYURL = "https://cdn.plot.ly/plotly-latest.min.js";
static const int64_t DEFAULT_PLOT_WIDTH = 1024;
static const int64_t DEFAULT_PLOT_HEIGHT = 768;

struct BenchArguments {
    bool show_help;
    int evaluations;
    std::string regex_filter;
    std::string scaling_str;
    bool is_list_only;
    std::string printer_arg;
    std::string plotly_url;
    int plot_width;
    int plot_height;
} g_bench_args;

static const ArgumentEntry benchArgs[] =
{ //  name            type        global variable             default value
  //  --------------  ----------- --------------------------- --------------
    {"-?",                ARG_BOOL,    &g_bench_args.show_help,      "0"},
    {"-h",                ARG_BOOL,    &g_bench_args.show_help,      "0"},
    {"-help",             ARG_BOOL,    &g_bench_args.show_help,      "0"},
    {"-evals",            ARG_INT,     &g_bench_args.evaluations,          std::to_string(DEFAULT_BENCH_EVALUATIONS)},
    {"-filter",           ARG_STRING,  &g_bench_args.regex_filter,   DEFAULT_BENCH_FILTER},
    {"-scaling",          ARG_STRING,  &g_bench_args.scaling_str,    DEFAULT_BENCH_SCALING},
    {"-list",             ARG_BOOL,    &g_bench_args.is_list_only,   "0"},
    {"-printer",          ARG_STRING,  &g_bench_args.printer_arg,    DEFAULT_BENCH_PRINTER},
    {"-plot-width",       ARG_INT,     &g_bench_args.plot_width,     std::to_string(DEFAULT_PLOT_WIDTH)},
    {"-plot-height",      ARG_INT,     &g_bench_args.plot_height,    std::to_string(DEFAULT_PLOT_HEIGHT)},
    {"-plot-plotlyurl",   ARG_STRING,  &g_bench_args.plotly_url,     DEFAULT_PLOT_PLOTLYURL},
};

void RegisterBenchArguments() {
    for (unsigned int i = 0; i < ARRAYLEN(benchArgs); i++) {
        gArgs.RegisterArg(benchArgs[i].name, &benchArgs[i]);
    }
}

int main(int argc, char** argv)
{
    RegisterBenchArguments();
    gArgs.ParseParameters(argc, argv);

    if (g_bench_args.show_help) {
        std::cout << HelpMessageGroup(_("Options:"))
                  << HelpMessageOpt("-?", _("Print this help message and exit"))
                  << HelpMessageOpt("-list", _("List benchmarks without executing them. Can be combined with -scaling and -filter"))
                  << HelpMessageOpt("-evals=<n>", strprintf(_("Number of measurement evaluations to perform. (default: %u)"), DEFAULT_BENCH_EVALUATIONS))
                  << HelpMessageOpt("-filter=<regex>", strprintf(_("Regular expression filter to select benchmark by name (default: %s)"), DEFAULT_BENCH_FILTER))
                  << HelpMessageOpt("-scaling=<n>", strprintf(_("Scaling factor for benchmark's runtime (default: %u)"), DEFAULT_BENCH_SCALING))
                  << HelpMessageOpt("-printer=(console|plot)", strprintf(_("Choose printer format. console: print data to console. plot: Print results as HTML graph (default: %s)"), DEFAULT_BENCH_PRINTER))
                  << HelpMessageOpt("-plot-plotlyurl=<uri>", strprintf(_("URL to use for plotly.js (default: %s)"), DEFAULT_PLOT_PLOTLYURL))
                  << HelpMessageOpt("-plot-width=<x>", strprintf(_("Plot width in pixel (default: %u)"), DEFAULT_PLOT_WIDTH))
                  << HelpMessageOpt("-plot-height=<x>", strprintf(_("Plot height in pixel (default: %u)"), DEFAULT_PLOT_HEIGHT));

        return 0;
    }

    SHA256AutoDetect();
    RandomInit();
    ECC_Start();
    SetupEnvironment();
    fPrintToDebugLog = false; // don't want to write to debug.log file

    double scaling_factor = boost::lexical_cast<double>(g_bench_args.scaling_str);

    std::unique_ptr<benchmark::Printer> printer(new benchmark::ConsolePrinter());
    if ("plot" == g_bench_args.printer_arg) {
        printer.reset(new benchmark::PlotlyPrinter(
            g_bench_args.plotly_url,
            g_bench_args.plot_width,
            g_bench_args.plot_height));
    }

    benchmark::BenchRunner::RunAll(*printer, g_bench_args.evaluations, scaling_factor, g_bench_args.regex_filter, g_bench_args.is_list_only);

    ECC_Stop();
}
