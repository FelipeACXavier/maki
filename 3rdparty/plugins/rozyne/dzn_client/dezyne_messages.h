#pragma once

// ===========================================================================================================
// Start simulation
// {
//   "filePath": "/home/felaze/Documents/PhD/Programs/behaviour_tree/monorepo/../examples/task.dzn",
//   "id": "019c8ae9-75f6-7039-983f-6b0035bbcc5c",
//   "importPath": [
//     "/home/felaze/Documents/PhD/Programs/behaviour_tree/monorepo/../",
//     "/home/felaze/Documents/PhD/Programs/behaviour_tree/monorepo/../examples"
//   ],
//   "name": "START_SIMULATION_COMMAND",
//   "rootArgs": {
//     "threads": "--threads=1"
//   },
//   "timestamp": "Mon, 23 Feb 2026 14:31:21 GMT",
//   "type": "START_SIMULATION_COMMAND",
//   "workingDirectory": "/home/felaze/Documents/PhD/Programs/behaviour_tree/monorepo"
// }
// ===========================================================================================================
// Simulation return
// {
//   "type": "SIMULATION_STATUS_RESPONSE",
//   "requestId": "019c8ae9-75f6-7039-983f-6b0035bbcc5c",
//   "success": true,
//   "result": {
//     "success": true,
//     "simulationId": "sim-123",
//     "browseUrl": "http://localhost:3000/trace",
//     "message": "Simulation Started Succesfully"
//   }
// }
// ===========================================================================================================
// Register sequence diagram
// {
//   "type": "REGISTER_SEQUENCE_DIAGRAM_VIEWER",
//   "simulationId": "sim-123"
// }
// ===========================================================================================================
// Trace update
// {
//   "type": "traceUpdate",
//   "simulationId": "sim-123",
//   "traceData": {
//     "trail": [],
//     "lifelines": [
//       {
//         "header": {
//           "instance": "api",
//           "role": "provides"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "api.trigger",
//             "role": "provides"
//           },
//           {
//             "text": "api.abort",
//             "role": "provides"
//           },
//           {
//             "text": "api.reset",
//             "role": "provides",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "sut.reject_flow.s0",
//           "role": "component"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "<back>",
//             "role": "component"
//           },
//           {
//             "text": "<defer>",
//             "role": "component",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "sut.reject_flow.abort_handler",
//           "role": "component"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "<back>",
//             "role": "component"
//           },
//           {
//             "text": "<defer>",
//             "role": "component",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "sut.ticket",
//           "role": "foreign"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "sut.ticket.create.success",
//             "role": "foreign",
//             "illegal": true
//           },
//           {
//             "text": "sut.ticket.create.failure",
//             "role": "foreign",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "sut.ticket.create",
//           "role": "requires"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "sut.ticket.create.success",
//             "role": "requires",
//             "illegal": true
//           },
//           {
//             "text": "sut.ticket.create.failure",
//             "role": "requires",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "sut.abort_flow.s0",
//           "role": "component"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "<back>",
//             "role": "component"
//           },
//           {
//             "text": "<defer>",
//             "role": "component",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "sut.abort_flow.s1",
//           "role": "component"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "<back>",
//             "role": "component"
//           },
//           {
//             "text": "<defer>",
//             "role": "component",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "sut.drive",
//           "role": "foreign"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "sut.drive.stop.success",
//             "role": "foreign",
//             "illegal": true
//           },
//           {
//             "text": "sut.drive.stop.failure",
//             "role": "foreign",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "sut.drive.stop",
//           "role": "requires"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "sut.drive.stop.success",
//             "role": "requires",
//             "illegal": true
//           },
//           {
//             "text": "sut.drive.stop.failure",
//             "role": "requires",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "sut.image",
//           "role": "foreign"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "sut.image.stop.success",
//             "role": "foreign",
//             "illegal": true
//           },
//           {
//             "text": "sut.image.stop.failure",
//             "role": "foreign",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "sut.image.stop",
//           "role": "requires"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "sut.image.stop.success",
//             "role": "requires",
//             "illegal": true
//           },
//           {
//             "text": "sut.image.stop.failure",
//             "role": "requires",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "arm1",
//           "role": "requires"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "arm1.success",
//             "role": "requires",
//             "illegal": true
//           },
//           {
//             "text": "arm1.failure",
//             "role": "requires",
//             "illegal": true
//           }
//         ]
//       },
//       {
//         "header": {
//           "instance": "arm2",
//           "role": "requires"
//         },
//         "activities": [],
//         "labels": [
//           {
//             "text": "arm2.success",
//             "role": "requires",
//             "illegal": true
//           },
//           {
//             "text": "arm2.failure",
//             "role": "requires",
//             "illegal": true
//           }
//         ]
//       }
//     ],
//     "events": [],
//     "states": [
//       [
//         {
//           "instance": "api",
//           "state": [
//             {
//               "name": "state",
//               "value": "State:Idle"
//             }
//           ]
//         },
//         {
//           "instance": "sut.reject_flow.s0",
//           "state": [
//             {
//               "name": "api.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "action1.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "action2.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "state",
//               "value": "State:Idle"
//             }
//           ]
//         },
//         {
//           "instance": "sut.reject_flow.abort_handler",
//           "state": [
//             {
//               "name": "api.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "action.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "handler_action.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "state",
//               "value": "State:State0"
//             }
//           ]
//         },
//         {
//           "instance": "sut.abort_flow.s0",
//           "state": [
//             {
//               "name": "api.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "action1.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "action2.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "state",
//               "value": "State:Idle"
//             }
//           ]
//         },
//         {
//           "instance": "sut.abort_flow.s1",
//           "state": [
//             {
//               "name": "api.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "action1.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "action2.state",
//               "value": "State:Idle"
//             },
//             {
//               "name": "state",
//               "value": "State:Idle"
//             }
//           ]
//         },
//         {
//           "instance": "sut.ticket.create",
//           "state": [
//             {
//               "name": "state",
//               "value": "State:Idle"
//             }
//           ]
//         },
//         {
//           "instance": "sut.drive.stop",
//           "state": [
//             {
//               "name": "state",
//               "value": "State:Idle"
//             }
//           ]
//         },
//         {
//           "instance": "sut.image.stop",
//           "state": [
//             {
//               "name": "state",
//               "value": "State:Idle"
//             }
//           ]
//         },
//         {
//           "instance": "arm1",
//           "state": [
//             {
//               "name": "state",
//               "value": "State:Idle"
//             }
//           ]
//         },
//         {
//           "instance": "arm2",
//           "state": [
//             {
//               "name": "state",
//               "value": "State:Idle"
//             }
//           ]
//         }
//       ]
//     ]
//   }
// }
// Start state view