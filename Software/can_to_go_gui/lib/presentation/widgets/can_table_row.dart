import 'package:can_to_go_gui/data/models/can_message_model.dart';
import 'package:flutter/material.dart';

class CanTableRow extends StatelessWidget {
  final CanMessageModel canMessage;
  final double screenWidth;
  final double screenHeight;
  const CanTableRow({
    Key? key,
    required this.canMessage,
    required this.screenWidth,
    required this.screenHeight,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Container(
      color: Colors.white,
      height: screenWidth * 0.025,
      width: screenWidth * 0.5,
      child: Row(
        children: [
          CanTableCell(text: canMessage.id.toString()),
          CanTableCell(text: canMessage.data.toString()),
          CanTableCell(text: canMessage.cycleTime.toString()),
        ],
      ),
    );
  }
}

class CanTableCell extends StatelessWidget {
  final String text;
  const CanTableCell({Key? key, required this.text}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 3.0),
      child: Text(
        text,
        style: const TextStyle(fontSize: 20.0),
      ),
    );
  }
}
