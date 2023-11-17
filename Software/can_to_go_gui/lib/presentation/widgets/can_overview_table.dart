import 'package:can_to_go_gui/data/models/can_message_model.dart';
import 'package:flutter/material.dart';
import 'package:can_to_go_gui/presentation/widgets/scrollable_table_view.dart';

class CanTable extends StatelessWidget {
  final List<String> headers;
  final List<CanMessageModel> canMessages;
  final bool isOverviewTab;
  final double rowHeight;

  const CanTable({
    Key? key,
    required this.canMessages,
    required this.headers,
    required this.isOverviewTab,
    required this.rowHeight,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    final List<List<String>> records = canMessages
        .map(
          (e) => isOverviewTab
              ? e.getCanOverviewStringList()
              : e.getCanTraceStringList(),
        )
        .toList();
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
      rows: records.map((record) {
        return TableViewRow(
          backgroundColor: Colors.white,
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
