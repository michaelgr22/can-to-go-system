import 'dart:async';
import 'package:can_to_go_gui/presentation/widgets/can_overview_table.dart';
import 'package:flutter/material.dart';

import 'package:can_to_go_gui/data/repositories/can_overview_repository.dart';
import 'package:can_to_go_gui/data/models/can_message_model.dart';

class CanOverviewTab extends StatefulWidget {
  final CanOverviewRepository _canOverviewRepository = CanOverviewRepository();

  CanOverviewTab({
    Key? key,
  }) : super(key: key);

  @override
  State<CanOverviewTab> createState() => _CanOverviewTabState();
}

class _CanOverviewTabState extends State<CanOverviewTab> {
  List<CanMessageModel> canMessagesUniqueId = [];
  late Timer _timer;

  @override
  void initState() {
    _updateMessages(true);
    super.initState();
    const oneSecond = Duration(seconds: 1);
    _timer = Timer.periodic(
      oneSecond,
      (Timer t) => _updateMessages(false),
    );
  }

  @override
  void dispose() {
    _timer.cancel(); // Cancel the timer to avoid calling setState after dispose
    super.dispose();
  }

  Future<void> _updateMessages(bool refresh) async {
    final updatedCanMessagesUniqueId =
        await widget._canOverviewRepository.getCanOverviewModels(refresh);
    if (mounted) {
      setState(() {
        canMessagesUniqueId = updatedCanMessagesUniqueId;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    final List<String> headers = [
      "ID",
      "DATA",
      "DLC",
      "CYCLE TIME",
    ];

    return CanTable(
      headers: headers,
      rowHeight: 60.0,
      isOverviewTab: true,
      canMessages: canMessagesUniqueId,
    );
  }
}
