#include <Rcpp.h>

#include "mapdeck_defaults.hpp"
#include "layers/layer_colours.hpp"
#include "spatialwidget/spatialwidget.hpp"
#include "sfheaders/df/sf.hpp"

Rcpp::List path_defaults(int n) {
	return Rcpp::List::create(
		_["stroke_colour"] = mapdeck::defaults::default_stroke_colour( n ),
		_["stroke_width"] = mapdeck::defaults::default_stroke_width( n ),
		_["dash_size"] = mapdeck::defaults::default_dash( n ),
		_["dash_gap"] = mapdeck::defaults::default_dash( n )
	);
}

Rcpp::List trips_defaults(int n) {
	return Rcpp::List::create(
		_["stroke_colour"] = mapdeck::defaults::default_stroke_colour( n ),
		_["stroke_width"] = mapdeck::defaults::default_stroke_width( n )
	);
}

Rcpp::List get_path_defaults( std::string layer_name, int data_rows ) {
	if( layer_name == "path" ) {
		return path_defaults( data_rows );
	}
	// else trips
	return trips_defaults( data_rows );
}

// [[Rcpp::export]]
Rcpp::List rcpp_path_geojson(
		Rcpp::DataFrame data,
		Rcpp::List params,
		Rcpp::StringVector unlist,
		int digits,
		std::string layer_name
	) {

	int data_rows = data.nrows();

	std::string sfc_column = data.attr("sf_column");

	Rcpp::List sfc = data[ sfc_column ];
	Rcpp::NumericMatrix sfc_coordinates = sfheaders::df::sfc_n_coordinates( sfc );

	Rcpp::DataFrame df = sfheaders::df::sf_to_df(
		data, sfc, sfc_column, sfc_coordinates, unlist, true
		);
	Rcpp::List geometry_cols(1);
	Rcpp::StringVector s = df.attr("sfc_columns");

	geometry_cols[0] = s;
	Rcpp::StringVector geom_names = {"geometry"};
	geometry_cols.names() = geom_names;

	int stride = s.length();

	Rcpp::NumericVector start_indices = sfc_coordinates( Rcpp::_, 0 );

	int i;
	Rcpp::StringVector param_names({"x","y","z","m"});

	for( i = 0; i < stride; ++i ) {
		Rcpp::String this_geom = s[i];
		Rcpp::String this_param = param_names[i];
		params[ this_param ] = this_geom;
	}

	Rcpp::List lst_defaults = get_path_defaults( layer_name, df.nrow() );

	std::unordered_map< std::string, std::string > path_colours = mapdeck::layer_colours::stroke_colours;
	Rcpp::StringVector path_legend = mapdeck::layer_colours::stroke_legend;
	Rcpp::StringVector parameter_exclusions = Rcpp::StringVector::create("legend","legend_options","palette","na_colour");

	std::string format = "rgb";

	Rcpp::List shape = spatialwidget::api::create_columnar(
		df,
		params,
		lst_defaults,
		path_colours,
		path_legend,
		data_rows,
		parameter_exclusions,
		geometry_cols,
		true,  // jsonify legend
		digits,
		format
	);

	Rcpp::List res(3);
	res[0] = shape;
	res[1] = start_indices;
	res[2] = stride;

	return res;

	// return spatialwidget::api::create_geojson(
	// 	data,
	// 	params,
	// 	lst_defaults,
	// 	path_colours,
	// 	path_legend,
	// 	data_rows,
	// 	parameter_exclusions,
	// 	geometry_columns,
	// 	true,   // jsonify legend
	// 	digits
	// );
}

// [[Rcpp::export]]
Rcpp::List rcpp_path_polyline(
		Rcpp::DataFrame data,
		Rcpp::List params,
		Rcpp::StringVector geometry_columns,
		std::string layer_name
	) {

	int data_rows = data.nrows();

	Rcpp::List lst_defaults = get_path_defaults( layer_name, data_rows );

	std::unordered_map< std::string, std::string > path_colours = mapdeck::layer_colours::stroke_colours;
	Rcpp::StringVector path_legend = mapdeck::layer_colours::stroke_legend;
	Rcpp::StringVector parameter_exclusions = Rcpp::StringVector::create("legend","legend_options","palette","na_colour");

	return spatialwidget::api::create_polyline(
		data,
		params,
		lst_defaults,
		path_colours,
		path_legend,
		data_rows,
		parameter_exclusions,
		geometry_columns,
		true  // jsonify legend
	);
}
