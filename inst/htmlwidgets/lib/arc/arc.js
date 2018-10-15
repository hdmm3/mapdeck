
function add_arc( map_id, arc_data, layer_id, auto_highlight ) {

  const arcLayer = new ArcLayer({
    id: 'arc-'+layer_id,
    data: arc_data,
    pickable: true,
    getStrokeWidth: d => d.stroke_width,
    getSourcePosition: d => decode_points( d.origin ),
    getTargetPosition: d => decode_points( d.destination ),
    getSourceColor: d => hexToRGBA( d.stroke_from, d.stroke_from_opacity ),
    getTargetColor: d => hexToRGBA( d.stroke_to, d.stroke_to_opacity ),
    onClick: info => layer_click( map_id, "arc", info ),
    onHover: updateTooltip,
    autoHighlight: auto_highlight,
    transitions: {
    	getSourceColor: 10000,
    	getTargetcolor: 10000,
    	getStrokeWidth: 1000
    }
  });

  update_layer( map_id, 'arc-'+layer_id, arcLayer );
}
