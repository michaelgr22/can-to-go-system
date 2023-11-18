import 'dart:async';
import 'package:can_to_go_gui/data/repositories/can_trace_repository.dart';
import 'package:can_to_go_gui/presentation/widgets/can_overview_table.dart';
import 'package:flutter/material.dart';

import 'package:can_to_go_gui/data/models/can_message_model.dart';

class CanTraceTab extends StatefulWidget {
  final CanTraceRepository _canTraceRepository = CanTraceRepository();

  CanTraceTab({Key? key}) : super(key: key);

  @override
  State<CanTraceTab> createState() => _CanTraceTabState();
}

class _CanTraceTabState extends State<CanTraceTab> {
  List<CanMessageModel> canMessages = [];
  int addedCount = 0;
  late Timer _timer;

  @override
  void initState() {
    super.initState();
    const oneSecond = Duration(seconds: 1);
    _timer = Timer.periodic(
      oneSecond,
      (Timer t) => _updateMessages(),
    );
  }

  @override
  void dispose() {
    _timer.cancel(); // Cancel the timer to avoid calling setState after dispose
    super.dispose();
  }

  Future<void> _updateMessages() async {
    final canTraceResult = await widget._canTraceRepository.getCanTraceModels();
    final updatedCanMessages = canTraceResult.canMessages;
    final updatedAddedCount = canTraceResult.addedCount;
    if (mounted) {
      setState(() {
        canMessages = updatedCanMessages;
        addedCount = updatedAddedCount;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    final List<String> headers = [
      "MICROS",
      "ID",
      "DATA",
      "DLC",
    ];

    return CanTable(
      headers: headers,
      rowHeight: 20.0,
      isOverviewTab: false,
      canMessages: canMessages,
      addedCount: addedCount,
    );
  }
}
