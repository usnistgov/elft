/*
 * Stick "NA" strings in numeric columns to bottom when sorting.
 */
var notApplicableType = $.fn.dataTable.absoluteOrderNumber([
	{
		value: 'Archived',
		position: 'bottom'
	},
	{
		value: 'Retired',
		position: 'bottom'
	},
	{
		value: 'NA',
		position: 'bottom'
	}
]);

$(document).ready(function() {
	$('#elft-table').DataTable({
		scrollX: false,
		dom: 'Bfrtip',
		buttons: ['colvis', 'csvHtml5'],
		order: [[5, "desc"]],
		columnDefs: [
			/*
			 * Date format
			 */
			{
				targets: 5,
				render: DataTable.render.datetime('DD MMM YYYY')
			},

			/*
			 * Visibility
			 */
			{
				targets: [1, 5, 7, 10, 50],
				visible: true
			},
			{
				targets: '_all',
				visible: false
			},

			/*
			 * Column types
			 */
			{
				targets: [1, 2, 3, 4, 6, 7, 8],
				type: "string"
			},
			{
				targets: 5,
				type: "date"
			},
			{
				targets: '_all',
				type: notApplicableType
			}
		],
		fixedColumns: {
			left: 1,
		},
		responsive: {
			details: {
				display: $.fn.dataTable.Responsive.display.modal({
					header: function (row) {
						var data = row.data();
						return 'Details for ' + data[0];
					}
				}),
				renderer: $.fn.dataTable.Responsive.renderer.tableAll({
					tableClass: 'ui table'
				})
			}
		}
	});
});
