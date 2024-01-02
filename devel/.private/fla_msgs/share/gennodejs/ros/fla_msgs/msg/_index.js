
"use strict";

let FlightEvent = require('./FlightEvent.js');
let ImageSegmentation = require('./ImageSegmentation.js');
let NodeList = require('./NodeList.js');
let Box = require('./Box.js');
let WaypointList = require('./WaypointList.js');
let NodeStatus = require('./NodeStatus.js');
let JoyDef = require('./JoyDef.js');
let Latency = require('./Latency.js');
let ProcessStatus = require('./ProcessStatus.js');
let FlightCommand = require('./FlightCommand.js');
let Keypoint = require('./Keypoint.js');
let ControlMessage = require('./ControlMessage.js');
let TelemString = require('./TelemString.js');
let FlightStateTransition = require('./FlightStateTransition.js');
let ImageDetections = require('./ImageDetections.js');
let Detection = require('./Detection.js');

module.exports = {
  FlightEvent: FlightEvent,
  ImageSegmentation: ImageSegmentation,
  NodeList: NodeList,
  Box: Box,
  WaypointList: WaypointList,
  NodeStatus: NodeStatus,
  JoyDef: JoyDef,
  Latency: Latency,
  ProcessStatus: ProcessStatus,
  FlightCommand: FlightCommand,
  Keypoint: Keypoint,
  ControlMessage: ControlMessage,
  TelemString: TelemString,
  FlightStateTransition: FlightStateTransition,
  ImageDetections: ImageDetections,
  Detection: Detection,
};
