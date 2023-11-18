import 'package:can_to_go_gui/data/models/can_message_model.dart';
import 'package:flutter/material.dart';
import 'package:can_to_go_gui/presentation/widgets/scrollable_table_view.dart';

// ignore: must_be_immutable
class CanTable extends StatelessWidget {
  final List<String> headers;
  final List<CanMessageModel> canMessages;
  final bool isOverviewTab;
  final double rowHeight;
  final int? addedCount;

  CanTable({
    Key? key,
    required this.canMessages,
    required this.headers,
    required this.isOverviewTab,
    required this.rowHeight,
    this.addedCount,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    double screenWidth = MediaQuery.of(context).size.width;
    return ScrollableTableView(
      headerBackgroundColor: Colors.grey,
      headers: headers.map((label) {
        return TableViewHeader(
          label: label,
          width: screenWidth / headers.length,
          minWidth: 100.0,
          labelFontSize: 20.0,
        );
      }).toList(),
      rows: canMessages.asMap().entries.map((entry) {
        int index = entry.key;
        CanMessageModel canMessage = entry.value;

        List<String> record = isOverviewTab
            ? canMessage.getCanOverviewStringList()
            : canMessage.getCanTraceStringList();

        bool isLastAdded = addedCount != null &&
            addedCount! < canMessages.length &&
            index <= addedCount!;
        Color rowColor = isLastAdded
            ? const Color.fromARGB(150, 135, 160, 204)
            : Colors.white;

        return TableViewRow(
          backgroundColor: rowColor,
          height: rowHeight,
          cells: record.map((value) {
            return TableViewCell(
              child: Text(value),
            );
          }).toList(),
        );
      }).toList(),
    );
  }
}
